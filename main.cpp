#define SDL_main main

// internal
#include "Vec3f.hpp"
#include "engine.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include "geometry.hpp"
#include "material.hpp"
#include "light.hpp"
// external
#include "CL/cl2.hpp"
// standard
#include <vector>

using namespace std;

int main() {

    // get opencl device to use
    vector<cl::Platform> platforms; cl::Platform::get(&platforms);
    vector<cl::Device> devices; platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device = devices[0];

    // create engine
    Engine *e = new Engine();
    // create window and assign it to engine
    Window *win = new Window(500, 300);
    // e->assign(win); win->show();

    // create scene
    Scene *scene = new Scene();
    scene->ambient(Vec3f(0, 0, 0));

    // set up camera
    unsigned int main_cam_id = scene->addCamera();
    scene->activateCamera(main_cam_id);
    scene->get_active_camera()->transform(Vec3f(0, 0, 0), Vec3f(0, 1, 0), Vec3f(0, 0, 1));
    scene->get_active_camera()->FOV(180);
    scene->get_active_camera()->antialiasing(1);
    // assign opencl device to camera
    scene->get_active_camera()->assign(device);
    // add lights
    unsigned int l1 = scene->addLight<PointLight>(new PointLightConfig(0, 0, -1, 0.5, 0.5, 0.5));
    unsigned int l2 = scene->addLight<PointLight>(new PointLightConfig(0, 3, -0.2, 0.5, 0.5, 0.5));
    // add materials
    unsigned int orange = scene->addMaterial<DiffuseMaterial>(new DiffuseMaterialConfig(0, 0, 0, 1, 1, 100, 0.5));
    unsigned int gray = scene->addMaterial<DiffuseMaterial>(new DiffuseMaterialConfig(0, 0, 0, 1, 1, 100, 0.5));
    // add geometries
    unsigned int s1 = scene->addGeometry<Sphere>(new SphereConfig(0, 1, 0, 0.5));
    unsigned int s2 = scene->addGeometry<Sphere>(new SphereConfig(0, 0, 100.5, 100));
    // assign materials to geometries
    scene->get_geometry(s1)->assign_material(orange);
    scene->get_geometry(s2)->assign_material(gray);

    // add scene to engine
    unsigned int scene_id = e->addScene(scene);
    e->activateScene(scene_id);
    // run engine
    // e->run();
    // save image to file
    scene->get_active_camera()->render_to_file("img/diffuse_gpu_2.bmp", 900, 500, 4);

    // destroy
    delete scene;
    delete win;
    delete e;

    return 0;
}