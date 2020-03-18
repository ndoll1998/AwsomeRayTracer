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
    scene->ambient(Vec3f(1.0, 1.0, 1.0));

    // set up camera
    unsigned int main_cam_id = scene->addCamera();
    scene->activateCamera(main_cam_id);
    scene->get_active_camera()->transform(Vec3f(0, 0, 0), Vec3f(0, 1, 0), Vec3f(0, 0, 1));
    scene->get_active_camera()->FOV(235);
    scene->get_active_camera()->antialiasing(5);
    // assign opencl device to camera
    scene->get_active_camera()->assign(device);

    // add lights
    // scene->addLight<PointLight>(new PointLightConfig(0.3, 0, -0.4, 0.5, 0.5, 0.5));
    // scene->addLight<PointLight>(new PointLightConfig(-0.3, 0, -0.4, 0.5, 0.5, 0.5));

    // add materials
    unsigned int diffA = scene->addMaterial<DiffuseMaterial>(new DiffuseMaterialConfig(0.8, 0.3, 0.3, 1, 0, 0));
    unsigned int diffB = scene->addMaterial<DiffuseMaterial>(new DiffuseMaterialConfig(0.8, 0.9, 1.0, 1, 0, 0));
    unsigned int metal = scene->addMaterial<MetalMaterial>(new MetalMaterialConfig(0.8, 0.6, 0.2, 1, 1, 100, 0.3));
    unsigned int dielec = scene->addMaterial<DielectricMaterial>(new DielectricMaterialConfig(1, 1, 100, 1.5f));

    // add geometries
    unsigned int ground = scene->addGeometry<Sphere>(new SphereConfig(0, 0, 100000.5, 100000));
    unsigned int left = scene->addGeometry<Sphere>(new SphereConfig(0.7, 1.2, 0, 0.5));
    unsigned int middle = scene->addGeometry<Sphere>(new SphereConfig(0, 0.8, 0, 0.5));
    unsigned int inner = scene->addGeometry<Sphere>(new SphereConfig(0, 0.8, 0, -0.49));
    unsigned int right = scene->addGeometry<Sphere>(new SphereConfig(-0.7, 1.2, 0, 0.5));
    // assign materials to geometries
    scene->get_geometry(ground)->assign_material(diffB);
    scene->get_geometry(left)->assign_material(diffA);
    scene->get_geometry(middle)->assign_material(dielec);
    scene->get_geometry(inner)->assign_material(dielec);
    scene->get_geometry(right)->assign_material(metal);

    // add scene to engine
    unsigned int scene_id = e->addScene(scene);
    e->activateScene(scene_id);
    // run engine
    e->run();
    // save image to file
    // scene->get_active_camera()->render_to_file("img/test.bmp", 900, 500, 4);

    // destroy
    delete scene;
    delete win;
    delete e;

    return 0;
}