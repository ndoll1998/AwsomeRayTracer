#include "Vec3f.hpp"

// forward declarations
class Scene;
class Geometry;

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
    Vec3f get_color(pair<Vec3f, Vec3f>* ray, float dist, Geometry* geo) const;
    Vec3f get_pixel_color(unsigned int i, unsigned int j, unsigned int w, unsigned int h) const;
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
    /* getters */
    Vec3f position(void) const { return this->pos_; }
    Vec3f direction(void) const { return this->dir_; }
    Vec3f up(void) const { return this->up_; }
    float FOV(void) const { return this->FOV_; }
    /* render */
    void render(void* pixels, unsigned int w, unsigned int h) const;
};