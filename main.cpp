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

    vector<int> a;
    int* b = a.data();

    // get opencl device to use
    vector<cl::Platform> platforms; cl::Platform::get(&platforms);
    vector<cl::Device> devices; platforms[1].getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device = devices[0];

    // create engine and window
    Engine *e = new Engine();
    Window *win = new Window(700, 500);
    // assign opencl device
    e->assignDevice(device);
    // assign and show window
    e->assignWindow(win);
    win->show();

    // create scene
    Scene *scene = new Scene();
    // set up camera
    unsigned int main_cam_id = scene->addCamera();
    scene->activateCamera(main_cam_id);
    scene->get_active_camera()->transform(
        Vec3f(0, 0, 0), Vec3f(0, 1, 0), Vec3f(0, 0, 1)
    );
    // add lights
    unsigned int l1 = scene->addLight<PointLight>(new PointLightConfig(0, 0, -1, 0.5, 0.5, 0.5));
    unsigned int l2 = scene->addLight<PointLight>(new PointLightConfig(0, 3, -0.2, 0.5, 0.5, 0.5));
    // add materials
    unsigned int blue = scene->addMaterial<ColorMaterial>(new ColorMaterialConfig(0, 0, 1, 1, 1, 50, 3));
    unsigned int red = scene->addMaterial<ColorMaterial>(new ColorMaterialConfig(1, 0, 0, 1, 1, 100, 3));
    // add geometries
    unsigned int s1 = scene->addGeometry<Sphere>(new SphereConfig(0.6, 2.5, 0, 0.7));
    unsigned int s2 = scene->addGeometry<Sphere>(new SphereConfig(-0.7, 2, 0, 0.4));
    // assign materials to geometries
    scene->get_geometry(s1)->assign_material(blue);
    scene->get_geometry(s2)->assign_material(red);

    // add scene to engine
    unsigned int scene_id = e->addScene(scene);
    e->activateScene(scene_id);
    // run engine
    e->run();

    // destroy
    delete scene;
    delete win;
    delete e;

    return 0;
}