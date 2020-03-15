#include <vector>

// forward declarations
class Window;
class Scene;
namespace cl {
    class Device;
    class Context;
    class CommandQueue;
    class Program;
    class Kernel;
};

// *** Engine Class ***

class Engine {

    private:
    /* window and scenes */
    Window *window;
    std::vector<Scene*>* scenes;
    Scene *active_scene;
    /* OpenCL */
    bool openCL_assigned = false;
    const cl::Device* device;
    cl::Context* context;
    cl::CommandQueue* queue;
    cl::Program* program;
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
    /* assign opencl device and window */
    void assignDevice(const cl::Device device);
    void assignWindow(Window *window);
    /* add and activate scenes */
    void activateScene(unsigned int scene_id);
    unsigned int addScene(Scene* scene);
    /* mainloop */
    void run(void);
};
