// external
#include "SDL2/SDL.h"
#include "CL/cl2.hpp"
// internal
#include "engine.hpp"
#include "window.hpp"
#include "scene.hpp"
#include "camera.hpp"
// standard
#include <time.h>
#include <fstream>
#include <iostream>

using namespace std;

/*** constructors ***/

Engine::Engine(void): running(false) {
    // initialize sdl
    SDL_Init(SDL_INIT_VIDEO);
    // create vectors
    this->scenes = new vector<Scene*>();
    // log
    cout << "Initialized engine" << endl;
}


/*** destructor ***/

Engine::~Engine(void){
    // quit sdl
    SDL_Quit();
    // destroy scene vector
    delete this->scenes;
    // destroy opencl if assigned
    if (this->openCL_assigned) {
        delete this->context;
        delete this->queue;
    }
    // log
    cout << "Destroyed engine" << endl;
}


/*** public methods ***/

void Engine::assignDevice(const cl::Device device) {
    // save reference to device
    this->device = &device;
    // create context and command-queue from device
    this->context = new cl::Context(device);
    this->queue = new cl::CommandQueue(device);
    // log
    cout << "Engine using device " << this->device->getInfo<CL_DEVICE_NAME>() << endl;
    // load opencl source files
    std::ifstream ray_source_file("src/kernels/camera.cl");
    cl::string ray_src(std::istreambuf_iterator<char>(ray_source_file), (std::istreambuf_iterator<char>()));
    // create program
    cl::Program::Sources source{ray_src};
    this->program = new cl::Program(*this->context, source);
    // build program
    if (this->program->build() != CL_BUILD_SUCCESS) {
        // show build log
        cout << this->program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(*this->device) << endl;
        throw;
    } else { cout << "Program build successful" << endl; }
    // set assigned
    this->openCL_assigned = true;
}

void Engine::assignWindow(Window *window) {
    // assign window
    this->window = window;
    // log
    cout << "Assigned window " << window->get_id() << " to engine" << endl;
}

void Engine::activateScene(unsigned int scene_id) {
    // activate scene
    this->active_scene = this->scenes->at(scene_id);
}

unsigned int Engine::addScene(Scene* scene) {
    // add scene to vector
    this->scenes->push_back(scene);
    // return scene id
    return this->scenes->size() - 1;
}


/*** mainloop ***/

void Engine::handle_events(void) {
    // handle events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // check for quit event
        if (event.type == SDL_QUIT) { this->running=false; }
    }
    return;
}

void Engine::update(void) {
}

void Engine::render(void) {
    // render scene on cpu
    this->active_scene->get_active_camera()->render(
        this->window->pixels(), this->window->get_width(), this->window->get_height()
    ); // display pixels
    this->window->display();
}

void Engine::run(void) {

    // get window size
    unsigned int w = this->window->get_width();
    unsigned int h = this->window->get_height();
    // kernel and buffers
    cl::Kernel* kern; 
    cl::Buffer* pixel_buf;
    // prepare opencl buffer
    if (this->openCL_assigned) {
        // set ray cast kernel
        kern = new cl::Kernel(*this->program, "camera_get_pixel_color");
        // set buffers
        pixel_buf = new cl::Buffer(*this->context, CL_MEM_WRITE_ONLY, h * w * 4);

        // set constant kernel arguments
        kern->setArg(0, *pixel_buf);
    }

    // start running engine
    this->running = true;

    // mainloop
    while (this->running) {
        // track time
        time_t start = clock();

        // handle events and update
        this->handle_events();
        this->update();

        if (this->openCL_assigned) {

            // update geometry buffers
            cl::Buffer geometry_buf(*this->context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, this->active_scene->get_geometry_compressor()->filled() * sizeof(float), this->active_scene->get_geometry_compressor()->data());
            cl::Buffer geometry_ids(*this->context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, this->active_scene->get_geometry_compressor()->n_instances() * sizeof(unsigned int), this->active_scene->get_geometry_compressor()->get_type_ids()->data());
            // set kernel arguments
            kern->setArg(1, geometry_buf);
            kern->setArg(2, geometry_ids);
            kern->setArg(3, this->active_scene->get_geometry_compressor()->n_instances());
            
            // update material buffers
            cl::Buffer material_buf(*this->context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, this->active_scene->get_material_compressor()->filled() * sizeof(float), this->active_scene->get_material_compressor()->data());
            cl::Buffer material_ids(*this->context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, this->active_scene->get_material_compressor()->n_instances() * sizeof(unsigned int), this->active_scene->get_material_compressor()->get_type_ids()->data());
            // set kernel arguments
            kern->setArg(4, material_buf);
            kern->setArg(5, material_ids);
            kern->setArg(6, this->active_scene->get_material_compressor()->n_instances());

            // update material buffers
            cl::Buffer light_buf(*this->context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, this->active_scene->get_light_compressor()->filled() * sizeof(float), this->active_scene->get_light_compressor()->data());
            cl::Buffer light_ids(*this->context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, this->active_scene->get_light_compressor()->n_instances() * sizeof(unsigned int), this->active_scene->get_light_compressor()->get_type_ids()->data());
            // set kernel arguments
            kern->setArg(7, light_buf);
            kern->setArg(8, light_ids);
            kern->setArg(9, this->active_scene->get_light_compressor()->n_instances());

            // set camera position
            Vec3f temp = this->active_scene->get_active_camera()->position();
            kern->setArg(10, temp.x()); kern->setArg(11, temp.y()); kern->setArg(12, temp.z());
            // set camera direction
            temp = this->active_scene->get_active_camera()->direction();
            kern->setArg(13, temp.x()); kern->setArg(14, temp.y()); kern->setArg(15, temp.z());
            // set camera up direction
            temp = this->active_scene->get_active_camera()->up();
            kern->setArg(16, temp.x()); kern->setArg(17, temp.y()); kern->setArg(18, temp.z());
            // set camera fov
            kern->setArg(19, this->active_scene->get_active_camera()->FOV());
            
            // render on opencl device
            this->queue->enqueueNDRangeKernel(*kern, cl::NullRange, cl::NDRange(h, w));
            this->queue->enqueueReadBuffer(*pixel_buf, CL_TRUE, 0, h * w * 4, this->window->pixels());
            this->queue->finish();
            // display pixels
            this->window->display();
        } else {
            // render on cpu
            this->render();
        }

        // log fps
        cout << "FPS: " << 1000 / (float)(clock() - start) << "\r"; cout.flush();
    }
}

