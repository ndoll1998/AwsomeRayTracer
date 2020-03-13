#include "Scene.hpp"
#include "Camera.hpp"
#include "geometry.hpp"
#include <tuple>
#include <iostream>

using namespace std;

/*** static members ***/

unsigned int Scene::global_id = 0;


/*** constructors ***/

Scene::Scene(void): id(Scene::global_id) {
    // increase global id
    Scene::global_id++;
    // create vector to store cameras
    this->cams = new vector<Camera*>();
    this->geometries = new vector<Geometry*>();
    // log
    cout << "Initialized scene " << this->id << endl;
}


/*** destructor ***/

Scene::~Scene(void) {
    // delete all instances in vectors
    for (Camera* cam : *this->cams) { delete cam; }
    for (Geometry* geometry : *this->geometries) { delete geometry; }
    // delete vectors
    delete this->cams;
    delete this->geometries;
    // log
    cout << "Destroyed scene " << this->id << endl;
}


/*** public methods ***/

void Scene::addGeometry(Geometry* geometry) {
    // add geometry to vector
    this->geometries->push_back(geometry);
}

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
    for (Geometry* geo : *this->geometries) {
        // cast intersection with geometry
        pair<bool, float> p = geo->cast(origin, dir);
        // check if geometry is closer
        if (p.first && (p.second < t)) {
            t = p.second; geometry = geo;
        }
    }
    // create return tuple
    return make_tuple<bool, float, Geometry*>(geometry != nullptr, move(t), move(geometry));
}


