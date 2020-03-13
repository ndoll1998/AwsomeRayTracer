#include "Vec3f.hpp"

// abstract geometry class

class Geometry {
    public:
    virtual std::pair<bool, float> cast(Vec3f origin, Vec3f dir) = 0;
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
    std::pair<bool, float> cast(Vec3f origin, Vec3f dir);

};