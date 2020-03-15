#include "Vec3f.hpp"
#include "memCompressor.hpp"
#include "_defines.h"

// forward declaration
class BaseMaterial;

// abstract geometry class

class Geometry : public Compressable {
    public:
    /* material */
    void assign_material(unsigned int material) { this->write(0, material); }
    unsigned int material(void) const { return this->read(0); }
    /* abstract methods */
    /* cast ray with geometry */
    virtual std::pair<bool, float> cast(const Vec3f origin, const Vec3f dir) const = 0;
    /* compute normal at given position */
    virtual Vec3f normal(const Vec3f p) const = 0;
};

// Sphere

class SphereConfig : public Config {
    public:
    /* position and radius */
    float x, y, z, r;
    /* constructor */
    SphereConfig(float x, float y, float z, float r);
};

class Sphere : public Geometry {

    private:
    /* getters */
    Vec3f get_center(void) const;
    float get_radius(void) const;
    /* setters */
    void set_center(float x, float y, float z);
    void set_radius(float r);

    public:
    /* Geometry Type ID and required size */
    unsigned int get_type_id(void) const { return GEOMETRY_SPHERE_TYPE_ID; }
    unsigned int get_size(void) const { return GEOMETRY_SPHERE_TYPE_SIZE; }
    /* apply config */
    void apply(Config* config);
    /* override geometry method */
    std::pair<bool, float> cast(const Vec3f origin, const Vec3f dir) const;
    Vec3f normal(Vec3f p) const;
};