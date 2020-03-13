#include "geometry.hpp"
#include "math.h"

/* constructors */

Sphere::Sphere(Vec3f center, float r): center_(center), radius_(r) {}
Sphere::Sphere(float x, float y, float z, float r): center_(x, y, z), radius_(r) {}

/* cast method */

std::pair<bool, float> Sphere::cast(Vec3f origin, Vec3f dir) {
    // analytic solution of sphere-ray-intersection
    Vec3f L = origin - this->center_;
    float a = Vec3f::dot(dir, dir);
    float b = 2 * Vec3f::dot(dir, L);
    float c = Vec3f::dot(L, L) - this->radius_ * this->radius_;
    
    float t;
    // solve quadratic function
    float discr = b * b - 4 * a * c;
    // no intersection
    if (discr < 0) return std::make_pair(false, 0);
    // exactly one intersection
    else if (discr == 0) { t = -0.5 * b / a; }
    // two intersections
    else {
        float q = (b > 0)?
            -0.5 * (b + sqrtf(discr)):
            -0.5 * (b - sqrtf(discr));
        t = std::min(q / a, c / q);
    }
    // return
    return std::make_pair(true, t);
}
