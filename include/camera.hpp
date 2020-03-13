#include "Vec3f.hpp"

// forward declarations
class Scene;

class Camera {
    private:
    /* reference to scene */
    const Scene *scene;
    /* local id in scene */
    const unsigned int id;
    /* transform and field of view */
    Vec3f pos_, dir_, up_, left_;
    float FOV_;

    /* private methods */
    std::pair<Vec3f,Vec3f> ray(unsigned int i, unsigned int j, unsigned int w, unsigned int h) const;

    public:
    /* constructors and destructor */
    Camera(Scene* scene, unsigned int id);
    ~Camera(void);
    /* transform */
    void transform(Vec3f pos, Vec3f dir, Vec3f up);
    /* setters */
    void position(Vec3f pos);
    void direction(Vec3f dir);
    void up(Vec3f up);
    void FOV(float FOV);
    /* render */
    void render(void* pixels, unsigned int w, unsigned int h) const;
};