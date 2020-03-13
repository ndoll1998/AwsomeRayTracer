#include "SDL2/SDL.h"
#include "camera.hpp"
#include "scene.hpp"
#include "geometry.hpp"
#include "material.hpp"
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

const Color Camera::get_pixel_color(unsigned int i, unsigned int j, unsigned int w, unsigned int h) const {
    // create ray through pixel and cast to all objects in scene
    pair<Vec3f, Vec3f> ray = this->ray(i, j, w, h);
    tuple<bool, Vec3f, Geometry*> intersect = this->scene->cast(ray.first, ray.second);
    // no intersection
    if (!get<0>(intersect)) return {0, 0, 0};
    // get intersection point and intersection geometry from tuple
    Vec3f ipoint = get<1>(intersect);
    Geometry* geo = get<2>(intersect);
    // compute color of intersection point
    Vec3f normal = geo->normal(ipoint);
    float t = -Vec3f::dot(ray.second, normal);
    // return color
    return this->scene->get_material(geo->material())->color(ipoint) * t;
}

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
            // get color of pixel
            Color c = this->get_pixel_color(x, y, w, h);
            // get values to override in pixel array
            int i = y * w + x;
            Uint8* base = ((Uint8 *)pixels) + (4 * i);
            // apply color to pixel
            base[0] = c.r();
            base[1] = c.g();
            base[2] = c.b();
            base[3] = 255;
       }
    }
}
