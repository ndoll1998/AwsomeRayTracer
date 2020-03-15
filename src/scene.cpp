// internal
#include "Scene.hpp"
#include "Camera.hpp"
#include "geometry.hpp"
#include "material.hpp"
#include "light.hpp"
// standard
#include <tuple>
#include <iostream>
#include <math.h>

using namespace std;

/*** static members ***/

unsigned int Scene::global_id = 0;


/*** constructors ***/

Scene::Scene(void): id(Scene::global_id) {
    // increase global id
    Scene::global_id++;
    // create compressors for matrials and geometries
    this->materialCompressor = new MemCompressor(100);
    this->geometryCompressor = new MemCompressor(100);
    this->lightCompressor = new MemCompressor(100);
    // create vector to store cameras
    this->cams = new vector<Camera*>();
    // log
    cout << "Initialized scene " << this->id << endl;
}


/*** destructor ***/

Scene::~Scene(void) {
    // delete compressors
    delete this->materialCompressor;
    delete this->geometryCompressor;
    delete this->lightCompressor;
    // delete all instances in vectors
    for (Camera* cam : *this->cams) { delete cam; }
    // delete vectors
    delete this->cams;
    // log
    cout << "Destroyed scene " << this->id << endl;
}


/*** public methods ***/

unsigned int Scene::addCamera(void) {
    // create camera object
    unsigned int cam_id = this->cams->size();
    Camera* cam = new Camera(this, cam_id);
    // add camera to vector
    this->cams->push_back(cam);
    // log
    cout << "Added camera " << cam_id << " to scene " << this->id << endl;
    // return camera id
    return cam_id;
}

void Scene::activateCamera(unsigned int cam_id) {
    // set active camera
    this->active_camera = this->cams->at(cam_id);
    // log
    cout << "Activated camera " << cam_id << " in scene " << this->id << endl;
}

tuple<bool, float, Geometry*> Scene::cast(const Vec3f origin, const Vec3f dir) const {

    float t = numeric_limits<float>::max(); Geometry* geometry = nullptr;
    // find intersection closest to origin
    for (Compressable* e : *this->geometryCompressor->get_instances()) {
        // convert compressable to geometry
        Geometry* geo = (Geometry*)e;
        // cast intersection with geometry
        pair<bool, float> p = geo->cast(origin, dir);
        // check if geometry is closer
        if (p.first && (p.second < t)) {
            t = p.second; geometry = geo;
        }
    }
    if (geometry != nullptr) {
        // create return tuple
        return make_tuple<bool, float, Geometry*>(true, move(t), move(geometry));
    }
    return make_tuple<bool, float, Geometry*>(false, 0.0, nullptr);
}

Vec3f Scene::light_color(Vec3f p, Vec3f vision_dir, Vec3f normal, Material* material) const {

    Vec3f light_color(0, 0, 0);
    // check for light
    for (Compressable* e : *this->get_light_compressor()->get_instances()) {
        // convert to light
        Light* l = (Light*)e;
        // create ray toward light and cast to scene
        Vec3f light_dir = l->light_direction(p);
        tuple<bool, float, Geometry*> light_cast = this->cast(p, light_dir);
        // unpack tuple
        bool intersect = get<0>(light_cast);
        float t = get<1>(light_cast);
        // on no intersection
        if ((!intersect) || (intersect && (t < 0))) { 
            // reflect light ray
            Vec3f light_reflect = light_dir.reflect(normal);
            // phong reflection model
            float diffuse = Vec3f::dot(light_dir, normal) * material->diffuse(p);
            float specular = pow(-Vec3f::dot(light_reflect, vision_dir), material->shininess(p)) * material->specular(p);
            // add all together
            light_color = light_color + l->color(p) * (diffuse + specular); 
        }
    }
    return light_color;
}
