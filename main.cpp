#define SDL_main main
#define CL_TARGET_OPENCL_VERSION 200

#include "Vec3f.hpp"
#include "engine.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include "geometry.hpp"
#include "material.hpp"

int main() {

    // create engine and window
    Engine *e = new Engine();
    Window *win = new Window(500, 500);
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
    // add materials
    unsigned int blue = scene->addMaterial<ColorMaterial>(new ColorMaterialConfig(0, 0, 1));
    unsigned int red = scene->addMaterial<ColorMaterial>(new ColorMaterialConfig(1, 0, 0));
    // add geometries
    unsigned int s1 = scene->addGeometry<Sphere>(new SphereConfig(0.3, 1.5, -0.4, 0.4));
    unsigned int s2 = scene->addGeometry<Sphere>(new SphereConfig(-0.2, 1, 0.2, 0.3));
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