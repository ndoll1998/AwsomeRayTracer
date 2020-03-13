#include <vector>
#include "vec3f.hpp"

// forward declarations
class BaseMaterial;
class Geometry;
class Camera;

class Scene {
    private:
    /* global scene id */
    static unsigned int global_id;
    /* cameras and objects */
    std::vector<BaseMaterial*>* materials;
    std::vector<Geometry*>* geometries;
    std::vector<Camera*> *cams;
    /* active camera */
    Camera* active_camera;
    /* scene members */
    const unsigned int id;

    public:
    /* constructors and destructor*/
    Scene(void);
    ~Scene(void);
    /* public methods */
    unsigned int addMaterial(BaseMaterial* material);
    unsigned int addGeometry(Geometry* geometry);
    unsigned int addCamera(void);
    void activateCamera(unsigned int);
    std::tuple<bool, Vec3f, Geometry*> cast(const Vec3f origin, const Vec3f dir) const;
    /* getters */
    Camera* get_active_camera(void) const { return this->active_camera; }
    BaseMaterial* get_material(unsigned int mat_id) const { return this->materials->at(mat_id); }
    Geometry* get_geometry(unsigned int geo_id) const { return this->geometries->at(geo_id); }
    Camera* get_camera(unsigned int cam_id) const { return this->cams->at(cam_id); }
    const unsigned int get_id(void) const { return this->id; }
};