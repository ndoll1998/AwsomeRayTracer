// external
#include "SDL2/SDL.h"
// internal
#include "engine.hpp"
#include "window.hpp"
#include "scene.hpp"
#include "camera.hpp"
// standard
#include <time.h>
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
    // log
    cout << "Destroyed engine" << endl;
}


/*** public methods ***/

void Engine::assign(Window *window) {
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

    // start running engine
    this->running = true;
    // prepare active cameras
    this->active_scene->get_active_camera()->prepare_rendering(
        this->window->get_width(), this->window->get_height()
    );

    // mainloop
    while (this->running) {
        // track time
        time_t start = clock();

        // handle events and update
        this->handle_events();
        this->update();
        // render active camera scene
        this->render();

        // log fps
        cout << "FPS: " << 1000 / (float)(clock() - start) << "\r"; cout.flush();
    }

    // clear active cameras
    this->active_scene->get_active_camera()->clear_rendering();
}

