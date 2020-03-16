#include <vector>

// forward declarations
class Window;
class Scene;

// *** Engine Class ***

class Engine {

    private:
    /* window and scenes */
    Window *window;
    std::vector<Scene*>* scenes;
    Scene *active_scene;
    /* engine members */
    bool running;

    /* mainloop functions */
    void handle_events(void);
    void update(void);
    void render(void);

    public:
    /* constructors and destructor */
    Engine(void);
    ~Engine(void);
    /* assign window and opencl device */
    void assign(Window *window);
    /* add and activate scenes */
    void activateScene(unsigned int scene_id);
    unsigned int addScene(Scene* scene);
    /* mainloop */
    void run(void);
};
