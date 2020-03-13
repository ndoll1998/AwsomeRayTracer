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
    // create vector to store cameras
    this->materials = new vector<BaseMaterial*>();
    this->geometries = new vector<Geometry*>();
    this->cams = new vector<Camera*>();
    // log
    cout << "Initialized scene " << this->id << endl;
}


/*** destructor ***/

Scene::~Scene(void) {
    // delete all instances in vectors
    for (Camera* cam : *this->cams) { delete cam; }
    for (Geometry* geo : *this->geometries) { delete geo; }
    for (BaseMaterial* mat : *this->materials) { delete mat; }
    // delete vectors
    delete this->cams;
    delete this->geometries;
    delete this->materials;
    // log
    cout << "Destroyed scene " << this->id << endl;
}


/*** public methods ***/

unsigned int Scene::addMaterial(BaseMaterial* material) {
    // add material to vector
    this->materials->push_back(material);
    // return id of material in scene
    return this->materials->size() - 1;
}

unsigned int Scene::addGeometry(Geometry* geometry) {
    // add geometry to vector
    this->geometries->push_back(geometry);
    // return id of geometry in scene
    return this->geometries->size() - 1;
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


tuple<bool, Vec3f, Geometry*> Scene::cast(const Vec3f origin, const Vec3f dir) const {

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
    if (geometry != nullptr) {
        // create return tuple
        Vec3f ipoint = origin + dir * t;
        return make_tuple<bool, Vec3f, Geometry*>(true, move(ipoint), move(geometry));
    }
    return make_tuple<bool, Vec3f, Geometry*>(false, Vec3f(), nullptr);
}


