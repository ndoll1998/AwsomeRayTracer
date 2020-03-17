// external
#include "SDL2/SDL.h"
#include "CL/cl2.hpp"
// internal
#include "camera.hpp"
#include "scene.hpp"
#include "geometry.hpp"
#include "material.hpp"
#include "light.hpp"
// standard
#include <tuple>
#include <iostream>
#include <fstream>
#include <math.h>
#include <time.h>

using namespace std;
using namespace cl;

/*** constructors ***/

Camera::Camera(Scene* scene, unsigned int id): scene(scene), id(id) {}


/*** destructors ***/

Camera::~Camera(void) {
    // log
    cout << "Destroyed camera " << this->id << " of scene " << this->scene->get_id() << endl;
    // destroy opencl if assigned
    if (this->openCL_assigned) {
        delete this->context;
        delete this->queue;
        delete this->program;
    }
}

/*** assign ***/

void Camera::assign(const Device device) {
    // save reference to device
    this->device = &device;
    // create context and command-queue from device
    cl_int err_;
    this->context = new cl::Context(device);
    this->queue = new cl::CommandQueue(*this->context);
    // log
    cout << "Camera " << this->id << " using device " << this->device->getInfo<CL_DEVICE_NAME>() << endl;
    // load opencl source files
    std::ifstream ray_source_file("src/kernels/camera.cl");
    cl::string ray_src(std::istreambuf_iterator<char>(ray_source_file), (std::istreambuf_iterator<char>()));
    // create program
    cl::Program::Sources source{ray_src};
    this->program = new cl::Program(*this->context, source);
    // build program
    if (this->program->build("-cl-std=CL2.0") != CL_BUILD_SUCCESS) {
        // show build log
        cout << this->program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(*this->device) << endl;
        throw;
    } else { cout << "Program build successful" << endl; }
    // set assigned
    this->openCL_assigned = true;
}

/*** transform ***/

void Camera::transform(Vec3f pos, Vec3f dir, Vec3f up) {
    this->position(pos); this->direction(dir); this->up(up);
}

/*** setters ***/
void Camera::position(Vec3f pos) { this->pos_ = pos; }
void Camera::direction(Vec3f dir) { this->dir_ = dir.normalize(); this->left_ = Vec3f::cross(this->dir_, this->up_); }
void Camera::up(Vec3f up) { this->up_ = up.normalize(); this->left_ = Vec3f::cross(this->dir_, this->up_); }
void Camera::FOV(float FOV) { this->FOV_ = FOV*3.14159265/180; }
void Camera::antialiasing(unsigned int n_samples) { this->n_samples = n_samples; }

/*** render ***/

Vec3f Camera::get_color(pair<Vec3f, Vec3f>* ray, unsigned int r_depth) const {
    // break recusion
    if (r_depth >= MAX_RECURSION_DEPTH) { return Vec3f(0, 0, 0); }
    // cast ray to scene
    tuple<bool, float, Geometry*> intersect = this->scene->cast(ray->first, ray->second);
    // unpack tuple
    bool hit = get<0>(intersect);
    float dist = get<1>(intersect);
    Geometry* geo = get<2>(intersect);
    // no intersection
    if (hit) {
        // get point of interest
        Vec3f p = ray->first + ray->second * (dist - EPS);
        // get material and normal
        Material* material = this->scene->get_material(geo->material());
        Vec3f normal = geo->normal(p);
        // get scatter ray
        pair<Vec3f, Vec3f> scattered;
        if (material->scatter(p, ray->second, normal, &scattered)) {
            // scatter and light color
            Vec3f scatter_color = this->get_color(&scattered, r_depth + 1);
            Vec3f light_color = this->scene->light_color(p, ray->second, normal, material);
            // get attenuation and compute color
            Vec3f attenuation = material->attenuation(p, ray->second, normal) * scatter_color;
            Vec3f color = attenuation * light_color * scatter_color;
            // return final color
            return color.clamp(0, 1);
        } else return Vec3f(0, 0, 0);
    } else { 
        // gradient background
        float t = 0.5 * (1.0 - ray->second.z());
        return Vec3f(1.0, 1.0, 1.0) * (1.0 - t) + Vec3f(0.5, 0.7, 1.0) * t; 
    }
}

Vec3f Camera::get_pixel_color(unsigned int i, unsigned int j, unsigned int w, unsigned int h) const {
    // initialize with ray throu middle of pixel
    pair<Vec3f, Vec3f> ray = this->ray(i, j, w, h);
    Vec3f color = get_color(&ray);
    // antialiasing
    for (int k = 0; k < this->n_samples - 1; k++) {
        // get random offset of pixel center
        float u = 2 * ((float)rand() / RAND_MAX) - 1;
        float v = 2 * ((float)rand() / RAND_MAX) - 1;
        // create ray throu random position in pixel
        ray = this->ray(i + u, j + v, w, h);
        // get color of ray
        color = color + get_color(&ray);
    }
    // return average color
    return color * (1.0 / this->n_samples);
}

pair<Vec3f, Vec3f> Camera::ray(float i, float j, unsigned int w, unsigned int h) const {
    // compute vertical field of view
    float vFOV = ((float)h / (float)w) * this->FOV_;
    // compute rotation of current ray
    float x_off = this->FOV_ * ((float)i / (float)w - 0.5);
    float y_off = vFOV * ((float)j / (float)h - 0.5);
    // rotate direction 
    Vec3f ray_dir = this->dir_ + this->left_ * x_off + this->up_ * y_off;
    // return ray
    return make_pair(this->pos_, ray_dir.normalize());
}

void Camera::render_cpu(void* pixels, unsigned int w, unsigned int h) const {
    // render each pixel
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            // get color of pixel
            Vec3f c = this->get_pixel_color(x, y, w, h);
            // get values to override in pixel array
            int i = y * w + x;
            Uint8* base = ((Uint8 *)pixels) + (4 * i);
            // apply color to pixel - apply gamma correction on pixels
            base[0] = int(sqrt(c.x()) * 255);
            base[1] = int(sqrt(c.y()) * 255);
            base[2] = int(sqrt(c.z()) * 255);
            base[3] = 255;
       }
    }
}

void Camera::prepare_rendering(unsigned int w, unsigned int h) {
    if (this->openCL_assigned) {
        // prepare opencl only if not yet initialized
        if (this->kern == nullptr) {
            // get kernel
            this->kern = new Kernel(*this->program, "camera_get_pixel_color");
            // create pixel buffer and set kernel argument
            this->pixel_buf = new Buffer(*this->context, CL_MEM_WRITE_ONLY, h * w * 4);
            this->kern->setArg(0, *this->pixel_buf);
            
            // prepare globals
            this->globals_buf = new Buffer(*this->context, CL_MEM_READ_WRITE, h * w * 8);
            Kernel prepare_kern(*this->program, "camera_prepare_globals");
            prepare_kern.setArg(0, *this->globals_buf);
            // run kernel
            this->queue->enqueueNDRangeKernel(prepare_kern, cl::NullRange, cl::NDRange(h, w));
            this->queue->finish();
            // set kernel argument
            this->kern->setArg(24, *this->globals_buf);
        }
    }
}

void Camera::clear_rendering(void) {
    // clear only if previously initialized
    if (this->kern != nullptr) {
        // clear kernel and buffers
        delete this->kern;
        delete this->pixel_buf;
        delete this->globals_buf;
    }
}

void Camera::render_gpu(void* pixels, unsigned int w, unsigned int h) const {

    // update geometry buffers
    cl::Buffer geometry_buf(*this->context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, this->scene->get_geometry_compressor()->filled() * sizeof(float), this->scene->get_geometry_compressor()->data());
    cl::Buffer geometry_ids(*this->context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, this->scene->get_geometry_compressor()->n_instances() * sizeof(unsigned int), this->scene->get_geometry_compressor()->get_type_ids()->data());
    // set kernel arguments
    this->kern->setArg(1, geometry_buf);
    this->kern->setArg(2, geometry_ids);
    this->kern->setArg(3, this->scene->get_geometry_compressor()->n_instances());

    // update material buffers
    cl::Buffer material_buf(*this->context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, this->scene->get_material_compressor()->filled() * sizeof(float), this->scene->get_material_compressor()->data());
    cl::Buffer material_ids(*this->context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, this->scene->get_material_compressor()->n_instances() * sizeof(unsigned int), this->scene->get_material_compressor()->get_type_ids()->data());
    // set kernel arguments
    this->kern->setArg(4, material_buf);
    this->kern->setArg(5, material_ids);
    this->kern->setArg(6, this->scene->get_material_compressor()->n_instances());

    // update material buffers
    cl::Buffer light_buf(*this->context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, this->scene->get_light_compressor()->filled() * sizeof(float), this->scene->get_light_compressor()->data());
    cl::Buffer light_ids(*this->context, CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, this->scene->get_light_compressor()->n_instances() * sizeof(unsigned int), this->scene->get_light_compressor()->get_type_ids()->data());
    // set kernel arguments
    this->kern->setArg(7, light_buf);
    this->kern->setArg(8, light_ids);
    this->kern->setArg(9, this->scene->get_light_compressor()->n_instances());

    // set camera position
    Vec3f temp = this->scene->get_active_camera()->position();
    this->kern->setArg(10, temp.x()); this->kern->setArg(11, temp.y()); this->kern->setArg(12, temp.z());
    // set camera direction
    temp = this->scene->get_active_camera()->direction();
    this->kern->setArg(13, temp.x()); this->kern->setArg(14, temp.y()); this->kern->setArg(15, temp.z());
    // set camera up direction
    temp = this->scene->get_active_camera()->up();
    this->kern->setArg(16, temp.x()); this->kern->setArg(17, temp.y()); this->kern->setArg(18, temp.z());
    // set camera fov and antialiasing values
    this->kern->setArg(19, this->scene->get_active_camera()->FOV());
    this->kern->setArg(20, this->scene->get_active_camera()->antialiasing());
    // set ambient light color
    this->kern->setArg(21, this->scene->ambient().x());
    this->kern->setArg(22, this->scene->ambient().y());
    this->kern->setArg(23, this->scene->ambient().z());

    // render on opencl device
    this->queue->enqueueNDRangeKernel(*this->kern, cl::NullRange, cl::NDRange(h, w));
    this->queue->enqueueReadBuffer(*this->pixel_buf, CL_TRUE, 0, h * w * 4, pixels);
    this->queue->finish();
}

void Camera::render(void* pixels, unsigned int w, unsigned int h) const {
    // render on gpu if assigned
    if (this->openCL_assigned) { this->render_gpu(pixels, w, h); }
    // render on cpu otherwise
    else { this->render_cpu(pixels, w, h); }
}

void Camera::render_to_file(const char* fname, int width, int height, int dpi) {
        // some values
        int k = width * height;
        int s = 4 * k;
        int n_bytes = 54 + s;

        // prepare rendering
        this->prepare_rendering(width, height);

        // log
        cout << "Rendering Image... "; cout.flush();
        // track time of rendering
        time_t start = clock();

        // render image
        char image[s];
        this->render(image, width, height);

        // log time needed for rendering
        cout << "Done (" << (clock() - start) / 1000.0 << "s)" << endl;

        // clean after render
        this->clear_rendering();

        double FACTOR = 39.375;
        int ppm = dpi * (int)FACTOR;

        // create file headers
        unsigned char file_header[14] = {'B', 'M', 0,0,0,0, 0,0,0,0, 54,0,0,0};
        unsigned char info_header[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};

        // fill file header
        file_header[ 2] = (unsigned char)(n_bytes);
        file_header[ 3] = (unsigned char)(n_bytes>>8);
        file_header[ 4] = (unsigned char)(n_bytes>>16);
        file_header[ 5] = (unsigned char)(n_bytes>>24);
        // fill info header
        info_header[ 4] = (unsigned char)(width);
        info_header[ 5] = (unsigned char)(width>>8);
        info_header[ 6] = (unsigned char)(width>>16);
        info_header[ 7] = (unsigned char)(width>>24);

        info_header[ 8] = (unsigned char)(height);
        info_header[ 9] = (unsigned char)(height>>8);
        info_header[10] = (unsigned char)(height>>16);
        info_header[11] = (unsigned char)(height>>24);

        info_header[21] = (unsigned char)(s);
        info_header[22] = (unsigned char)(s>>8);
        info_header[23] = (unsigned char)(s>>16);
        info_header[24] = (unsigned char)(s>>24);

        info_header[25] = (unsigned char)(ppm);
        info_header[26] = (unsigned char)(ppm>>8);
        info_header[27] = (unsigned char)(ppm>>16);
        info_header[28] = (unsigned char)(ppm>>24);

        info_header[30] = (unsigned char)(3);

        // open file
        std::FILE* file; file = fopen(fname, "wb");

        // write headers to file
        fwrite(file_header, 1, 14, file);
        fwrite(info_header, 1, 40, file);

        // write image - read bgr from rgba format
        for (int i = k-1; i >= 0; i--) {
            fwrite(image + i * 4 + 2, 1, 1, file);  // blue
            fwrite(image + i * 4 + 1, 1, 1, file);  // green
            fwrite(image + i * 4 + 0, 1, 1, file);  // red
        }
        // close file
        fclose(file);

        // log
        cout << "Saved image: " << fname << endl;
}
