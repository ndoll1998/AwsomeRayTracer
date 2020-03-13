#include <vector>
#include "vec3f.hpp"

// forward declarations
class Camera;
class Geometry;

class Scene {
    private:
    /* global scene id */
    static unsigned int global_id;
    /* cameras and objects */
    std::vector<Geometry*>* geometries;
    std::vector<Camera*> *cams;
    Camera* active_camera;
    /* scene members */
    const unsigned int id;

    public:
    /* constructors and destructor*/
    Scene(void);
    ~Scene(void);
    
    /* public methods */
    void addGeometry(Geometry* geometry);
    unsigned int addCamera(void);
    void activateCamera(unsigned int);
    std::tuple<bool, float, Geometry*> cast(const Vec3f origin, const Vec3f dir) const;
    /* getters */
    Camera* get_active_camera(void) const { return this->active_camera; }
    Camera* get_camera(unsigned int cam_id) const { return this->cams->at(cam_id); }
    const unsigned int get_id(void) const { return this->id; }
};