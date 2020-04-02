#include "Vec3f.hpp"
#include "memCompressor.hpp"
#include "_defines.h"

// forward declaration
class Material;

// abstract geometry class

class Geometry : public Compressable {
    public:
    /* material */
    void assign_material(unsigned int material) { this->write(0, material); }
    unsigned int material(void) const { return this->read(0); }
    /* abstract methods */
    /* cast ray with geometry */
    virtual bool cast(const Vec3f origin, const Vec3f dir, float* t) const = 0;
    /* compute normal at given position */
    virtual Vec3f normal(const Vec3f p) const = 0;
};

// Sphere

class SphereConfig : public Config {
    public:
    /* position and radius */
    Vec3f center; float r;
    /* constructor */
    SphereConfig(Vec3f center, float r);
};

class Sphere : public Geometry {

    private:
    /* getters */
    Vec3f get_center(void) const;
    float get_radius(void) const;
    /* setters */
    void set_center(Vec3f center);
    void set_radius(float r);

    public:
    /* Geometry Type ID and required size */
    unsigned int get_type_id(void) const { return GEOMETRY_SPHERE_TYPE_ID; }
    unsigned int get_size(void) const { return GEOMETRY_SPHERE_TYPE_SIZE; }
    /* apply config */
    void apply(Config* config);
    /* override geometry method */
    bool cast(const Vec3f origin, const Vec3f dir, float* t) const;
    Vec3f normal(Vec3f p) const;
};

// Plane

class PlaneConfig : public Config {
    public:
    /* origin and normal */
    Vec3f origin, normal;
    /* constructor */
    PlaneConfig(Vec3f origin, Vec3f normal);
};

class Plane : public Geometry {

    private:
    /* getters */
    Vec3f get_origin(void) const;
    Vec3f get_normal(void) const;
    /* setters */
    void set_origin(Vec3f center);
    void set_normal(Vec3f normal);

    public:
    /* Geometry Type ID and required size */
    unsigned int get_type_id(void) const { return GEOMETRY_PLANE_TYPE_ID; }
    unsigned int get_size(void) const { return GEOMETRY_PLANE_TYPE_SIZE; }
    /* apply config */
    void apply(Config* config);
    /* override geometry method */
    bool cast(const Vec3f origin, const Vec3f dir, float* t) const;
    Vec3f normal(Vec3f p) const;
};


// Triangle

class TriangleConfig : public Config {
    public:
    /* origin and normal */
    Vec3f A, B, C;
    /* constructor */
    TriangleConfig(Vec3f A, Vec3f B, Vec3f C);
};

class Triangle : public Plane {

    private:
    /* getters */
    Vec3f get_A(void) const;
    Vec3f get_B(void) const;
    Vec3f get_C(void) const;
    /* setters */
    void set_A(Vec3f A);
    void set_B(Vec3f B);
    void set_C(Vec3f C);

    public:
    /* Geometry Type ID and required size */
    unsigned int get_type_id(void) const { return GEOMETRY_TRIANGLE_TYPE_ID; }
    unsigned int get_size(void) const { return GEOMETRY_TRIANGLE_TYPE_SIZE; }
    /* apply config */
    void apply(Config* config);
    /* override geometry method */
    bool cast(const Vec3f origin, const Vec3f dir, float* t) const;
    Vec3f normal(Vec3f p) const;
};
