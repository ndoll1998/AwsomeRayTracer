#include "window.hpp"
#include <iostream>

using namespace std;

/*** static members ***/

unsigned int Window::global_id = 0;

/*** constructors ***/

Window::Window(unsigned int w, unsigned int h): width(w), height(h), id(Window::global_id) {
    // create a window
    this->window = SDL_CreateWindow("Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_HIDDEN);
    // get texture and renderer from window
    this->renderer = SDL_CreateRenderer(this->window, -1, 0);
    this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
    // increase gloabl window id
    Window::global_id++;
    // log
    cout << "Initialized window " << this->id << endl;
}

/*** destructor ***/

Window::~Window(void) {
    // destroy all
    SDL_DestroyTexture(this->texture);
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    // log
    cout << "Destroyed window " << this->id << endl;
}

/*** public methods ***/

void Window::show(void) { SDL_ShowWindow(this->window); }

void* Window::pixels(void) const {
    // lock texture to manipulate
    void* pixels; int pitch;
    SDL_LockTexture(this->texture, NULL, &pixels, &pitch);
    // return pixels
    return pixels;
}

void Window::display(void) {
    // show new texture
    SDL_UnlockTexture(this->texture);
    SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);
    // display renderer
    SDL_RenderPresent(this->renderer);
}
