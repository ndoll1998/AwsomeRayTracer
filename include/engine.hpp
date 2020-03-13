#include <vector>

// forward declaration
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

    public:
    /* constructors and destructor */
    Engine(void);
    ~Engine(void);

    /* public methods */
    void assignWindow(Window *window);
    void activateScene(unsigned int scene_id);
    unsigned int addScene(Scene* scene);
    /* mainloop */
    void handle_events(void);
    void update(void);
    void render(void);
    void run(void);
};
