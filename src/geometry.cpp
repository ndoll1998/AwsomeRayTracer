#include "geometry.hpp"
#include "math.h"

/* Sphere */

// Config
SphereConfig::SphereConfig(float x, float y, float z, float r): x(x), y(y), z(z), r(r) {}

// getters
Vec3f Sphere::get_center(void) const { return Vec3f(this->read(0), this->read(1), this->read(2)); }
float Sphere::get_radius(void) const { return this->read(3); }
// setters
void Sphere::set_center(float x, float y, float z) { this->write(0, x); this->write(1, y); this->write(2, z); }
void Sphere::set_radius(float r) { this->write(3, r); }

// apply config
void Sphere::apply(Config* config) {
    // convert config
    SphereConfig* config_ = (SphereConfig*)config;
    // apply values from configuration
    this->set_center(config_->x, config_->y, config_->z);
    this->set_radius(config_->r);
}

// ray-cast method
std::pair<bool, float> Sphere::cast(const Vec3f origin, const Vec3f dir) const {
    // get center and radius
    Vec3f center = this->get_center();
    float radius = this->get_radius();
    // analytic solution of sphere-ray-intersection
    Vec3f L = origin - center;
    float a = Vec3f::dot(dir, dir);
    float b = 2 * Vec3f::dot(dir, L);
    float c = Vec3f::dot(L, L) - radius * radius;
    
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

// normal at specified position on surface
Vec3f Sphere::normal(const Vec3f p) const {
    // compute normal vector
    Vec3f n = p - this->get_center(); n.normalize();
    return n;
}