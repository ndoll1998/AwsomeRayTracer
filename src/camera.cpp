// external
#include "SDL2/SDL.h"
// internal
#include "camera.hpp"
#include "scene.hpp"
#include "geometry.hpp"
#include "material.hpp"
#include "light.hpp"
// standard
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
void Camera::direction(Vec3f dir) { this->dir_ = dir.normalize(); this->left_ = Vec3f::cross(this->dir_, this->up_); }
void Camera::up(Vec3f up) { this->up_ = up.normalize(); this->left_ = Vec3f::cross(this->dir_, this->up_); }
void Camera::FOV(float FOV) { this->FOV_ = FOV*3.14159265/180; }

/*** render ***/

Vec3f Camera::get_color(pair<Vec3f, Vec3f>* ray, float dist, Geometry* geo) const {
    // get point of interest
    Vec3f p = ray->first + ray->second * (dist - EPS);
    // get material of geometry
    Material* material = this->scene->get_material(geo->material());
    // get normal and reflection of ray on geometry
    Vec3f normal = geo->normal(p);
    Vec3f reflect = ray->second.reflect(normal);
    // get base color from material
    Vec3f base_color = material->color(p);
    Vec3f light_color = this->scene->light_color(p, ray->second, normal, material);
    // return color
    return base_color * light_color;
}

Vec3f Camera::get_pixel_color(unsigned int i, unsigned int j, unsigned int w, unsigned int h) const {
    // create ray throu pixel and cast to all objects in scene
    pair<Vec3f, Vec3f> ray = this->ray(i, j, w, h);
    tuple<bool, float, Geometry*> intersect = this->scene->cast(ray.first, ray.second);
    // unpack tuple
    bool hit = get<0>(intersect);
    float dist = get<1>(intersect);
    Geometry* geo = get<2>(intersect);
    // no intersection
    if (!hit || (dist < 0)) return Vec3f(0, 0, 0);
    // get color
    return this->get_color(&ray, dist, geo).clamp(0, 1);
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
    return make_pair(this->pos_, ray_dir);
}

void Camera::render(void* pixels, unsigned int w, unsigned int h) const {
    // fill texture with rendered image
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            // get color of pixel
            Vec3f c = this->get_pixel_color(x, y, w, h);
            // get values to override in pixel array
            int i = y * w + x;
            Uint8* base = ((Uint8 *)pixels) + (4 * i);
            // apply color to pixel
            base[0] = c.x() * 255;
            base[1] = c.y() * 255;
            base[2] = c.z() * 255;
            base[3] = 255;
       }
    }
}
