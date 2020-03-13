#include "Vec3f.hpp"

// forward declaration
class BaseMaterial;

// abstract geometry class

class Geometry {
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

class Sphere : public Geometry {

    private:
    /* position and radius */
    Vec3f center_;
    float radius_;

    public:
    /* constructors */
    Sphere(Vec3f center, float r);
    Sphere(float x, float y, float z, float r);
    /* override cast method */
    std::pair<bool, float> cast(const Vec3f origin, const Vec3f dir) const;
    Vec3f normal(Vec3f p) const;
};