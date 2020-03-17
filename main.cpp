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
    vector<cl::Device> devices; platforms[1].getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device = devices[0];

    // create engine
    Engine *e = new Engine();
    // create window and assign it to engine
    Window *win = new Window(500, 300);
    e->assign(win); win->show();

    // create scene
    Scene *scene = new Scene();
    scene->ambient(Vec3f(1, 1, 1));

    // set up camera
    unsigned int main_cam_id = scene->addCamera();
    scene->activateCamera(main_cam_id);
    scene->get_active_camera()->transform(Vec3f(0, 0, 0), Vec3f(0, 1, 0), Vec3f(0, 0, 1));
    scene->get_active_camera()->FOV(235);
    scene->get_active_camera()->antialiasing(1);
    // assign opencl device to camera
    scene->get_active_camera()->assign(device);

    // add lights
    // scene->addLight<PointLight>(new PointLightConfig(0.3, 0, -0.4, 0.5, 0.5, 0.5));
    // scene->addLight<PointLight>(new PointLightConfig(-0.3, 0, -0.4, 0.5, 0.5, 0.5));

    // add materials
    unsigned int diffA = scene->addMaterial<DiffuseMaterial>(new DiffuseMaterialConfig(0.8, 0.3, 0.3, 1, 0, 0));
    unsigned int diffB = scene->addMaterial<DiffuseMaterial>(new DiffuseMaterialConfig(0.8, 0.8, 0, 1, 0, 0));
    unsigned int metalA = scene->addMaterial<MetalMaterial>(new MetalMaterialConfig(0.8, 0.8, 0.8, 1, 1, 100, 0.3));
    unsigned int metalB = scene->addMaterial<MetalMaterial>(new MetalMaterialConfig(0.8, 0.8, 0.8, 1, 1, 100, 1.0));

    // add geometries
    unsigned int s1 = scene->addGeometry<Sphere>(new SphereConfig(0, 1, 0, 0.5));
    unsigned int s2 = scene->addGeometry<Sphere>(new SphereConfig(0, 0, 100.5, 100));
    unsigned int s3 = scene->addGeometry<Sphere>(new SphereConfig(1, 1, 0, 0.5));
    unsigned int s4 = scene->addGeometry<Sphere>(new SphereConfig(-1, 1, 0, 0.5));
    // assign materials to geometries
    scene->get_geometry(s1)->assign_material(diffA);
    scene->get_geometry(s2)->assign_material(diffB);
    scene->get_geometry(s3)->assign_material(metalA);
    scene->get_geometry(s4)->assign_material(metalB);

    // add scene to engine
    unsigned int scene_id = e->addScene(scene);
    e->activateScene(scene_id);
    // run engine
    // e->run();
    // save image to file
    scene->get_active_camera()->render_to_file("img/metal_gpu.bmp", 900, 500, 4);

    // destroy
    delete scene;
    delete win;
    delete e;

    return 0;
}