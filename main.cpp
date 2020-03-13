#include "Vec3f.hpp"
#include "engine.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include "geometry.hpp"
#include <iostream>

#define SDL_main main

using namespace std;

int main() 
{
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
    // add geometries
    scene->addGeometry(new Sphere(0, 1, 0, 0.3));

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