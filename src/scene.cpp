#include "Scene.hpp"
#include "Camera.hpp"
#include "geometry.hpp"
#include "material.hpp"
#include <tuple>
#include <iostream>

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


tuple<bool, Vec3f, Geometry*> Scene::cast(const Vec3f origin, const Vec3f dir) const {

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
        Vec3f ipoint = origin + dir * t;
        return make_tuple<bool, Vec3f, Geometry*>(true, move(ipoint), move(geometry));
    }
    return make_tuple<bool, Vec3f, Geometry*>(false, Vec3f(), nullptr);
}


