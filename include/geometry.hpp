#include "Vec3f.hpp"
#include "memCompressor.hpp"

// forward declaration
class BaseMaterial;

// abstract geometry class

class Geometry : public Compressable {
    private:
    /* material id */
    unsigned int material_;

    public:
    /* material */
    void assign_material(unsigned int material) { this->material_ = material; }
    unsigned int material(void) const { return this->material_; }
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
    const unsigned int get_type_id(void) const { return 0; }
    const unsigned int get_size(void) const { return 4; }
    /* apply config */
    void apply(Config* config);
    /* override geometry method */
    std::pair<bool, float> cast(const Vec3f origin, const Vec3f dir) const;
    Vec3f normal(Vec3f p) const;
};