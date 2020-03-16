#include "Vec3f.hpp"

// forward declarations
class Scene;
class Geometry;
namespace cl {
    class Device;
    class Context;
    class CommandQueue;
    class Program;
    class Kernel;
    class Buffer;
};

class Camera {
    private:
    /* reference to scene */
    const Scene *scene;
    /* local id in scene */
    const unsigned int id;
    
    /* transform and field of view */
    Vec3f pos_, dir_, up_, left_;
    float FOV_ = 60.0*3.14159265/180;
    /* anti-aliasing */
    unsigned int n_samples = 1;

    /* OpenCL set up */
    bool openCL_assigned = false;
    const cl::Device* device;
    cl::Context* context;
    cl::CommandQueue* queue;
    cl::Program* program;
    /* OpenCL helpers */
    cl::Kernel* kern = nullptr;
    cl::Buffer* pixel_buf = nullptr;
    cl::Buffer* globals_buf = nullptr;

    /* private methods */
    Vec3f get_color(pair<Vec3f, Vec3f>* ray, unsigned int r_depth = 0) const;
    Vec3f get_pixel_color(unsigned int i, unsigned int j, unsigned int w, unsigned int h) const;
    std::pair<Vec3f,Vec3f> ray(float i, float j, unsigned int w, unsigned int h) const;
    /* private render methods */
    void render_cpu(void* pixels, unsigned int w, unsigned int h) const;
    void render_gpu(void* pixels, unsigned int w, unsigned int h) const;

    public:
    /* constructors and destructor */
    Camera(Scene* scene, unsigned int id);
    ~Camera(void);
    /* assign open-cl device to camera */
    void assign(const cl::Device device);
    /* transform */
    void transform(Vec3f pos, Vec3f dir, Vec3f up);
    /* setters */
    void position(Vec3f pos);
    void direction(Vec3f dir);
    void up(Vec3f up);
    void FOV(float FOV);
    void antialiasing(unsigned int n_samples);
    /* getters */
    Vec3f position(void) const { return this->pos_; }
    Vec3f direction(void) const { return this->dir_; }
    Vec3f up(void) const { return this->up_; }
    float FOV(void) const { return this->FOV_; }
    unsigned int antialiasing(void) const { return this->n_samples; }
    /* render */
    void render(void* pixels, unsigned int w, unsigned int h) const;
    void render_to_file(const char* fname, int width, int height, int dpi);
    /* prepare and clear rendering */
    void prepare_rendering(unsigned int w, unsigned int h);
    void clear_rendering(void);
};