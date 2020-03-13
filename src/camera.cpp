#include "SDL2/SDL.h"
#include "camera.hpp"
#include "scene.hpp"
#include <tuple>
#include <iostream>

using namespace std;

/*** constructors ***/

Camera::Camera(Scene* scene, unsigned int id): scene(scene), id(id) {
    // set field of view
    this->FOV(90.0f);
}


/*** destructors ***/

Camera::~Camera(void) {
    // log
    cout << "Destroyed camera " << this->id << " of scene " << this->scene->get_id() << endl;
}

/*** transform ***/

void Camera::transform(Vec3f pos, Vec3f dir, Vec3f up) {
    this->position(pos); this->direction(dir); this->up(up);
}

/*** setters ***/
void Camera::position(Vec3f pos) { this->pos_ = pos; }
void Camera::direction(Vec3f dir) { this->dir_ = dir; this->dir_.normalize(); }
void Camera::up(Vec3f up) { this->up_ = up; this->up_.normalize(); this->left_ = Vec3f::cross(this->dir_, this->up_); }
void Camera::FOV(float FOV) { this->FOV_ = FOV*3.14159265/180; }

/*** render ***/

pair<Vec3f, Vec3f> Camera::ray(unsigned int i, unsigned int j, unsigned int w, unsigned int h) const {
    // compute vertical field of view
    float vFOV = ((float)h / (float)w) * this->FOV_;
    // compute rotation of current ray
    float xrot = this->FOV_ * ((float)i / (float)w - 0.5);
    float yrot = vFOV * ((float)j / (float)h - 0.5);
    // rotate direction 
    Vec3f ray_dir = this->dir_.rotate(this->up_, xrot).rotate(this->left_, yrot);
    // return ray
    return std::make_pair(this->pos_, ray_dir);
}

void Camera::render(void* pixels, unsigned int w, unsigned int h) const {
    // fill texture with rendered image
    #pragma omp parallel for
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            // compute index
            int i = y * w + x;
            // write rgb-values
            Uint8* base = ((Uint8 *)pixels) + (4 * i);

            // create ray
            pair<Vec3f, Vec3f> ray = this->ray(x, y, w, h);
            tuple<bool, float, Geometry*> intersect = this->scene->cast(ray.first, ray.second);

            if (get<0>(intersect)) {
                base[0] = 255 * (1 - get<1>(intersect));
                base[1] = 255 * (1 - get<1>(intersect));
                base[2] = 255 * (1 - get<1>(intersect));
            } else {
                base[0] = 0;
                base[1] = 0;
                base[2] = 0;
            }
            base[3] = 255;
       }
    }
}
