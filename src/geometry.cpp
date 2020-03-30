#include "geometry.hpp"
#include "math.h"

/* Sphere */

// Config
SphereConfig::SphereConfig(Vec3f center, float r): center(center), r(r) {}

// getters
Vec3f Sphere::get_center(void) const { return Vec3f(this->read(1), this->read(2), this->read(3)); }
float Sphere::get_radius(void) const { return this->read(4); }
// setters
void Sphere::set_center(Vec3f center) { this->write(1, center.x()); this->write(2, center.y()); this->write(3, center.z()); }
void Sphere::set_radius(float r) { this->write(4, r); }

// apply config
void Sphere::apply(Config* config) {
    // convert config
    SphereConfig* config_ = (SphereConfig*)config;
    // apply values from configuration
    this->set_center(config_->center);
    this->set_radius(config_->r);
}

// ray-cast method
bool Sphere::cast(const Vec3f origin, const Vec3f dir, float* t) const {
    // get center and radius
    Vec3f center = this->get_center();
    float radius = this->get_radius();
    // analytic solution of sphere-ray-intersection
    Vec3f L = origin - center;
    float a = Vec3f::dot(dir, dir);
    float b = 2 * Vec3f::dot(dir, L);
    float c = Vec3f::dot(L, L) - radius * radius;

    // solve quadratic function
    float discr = b * b - 4 * a * c;
    // no intersection
    if (discr < 0) return false;
    // exactly one intersection
    else if (discr == 0) { *t = -0.5 * b / a; }
    // two intersections
    else {
        float q = (b > 0)?
            -0.5 * (b + sqrtf(discr)):
            -0.5 * (b - sqrtf(discr));
        *t = std::min(q / a, c / q);
    }
    // return
    return *t > 0;
}

// normal at specified position on surface
Vec3f Sphere::normal(const Vec3f p) const {
    // compute normal vector
    Vec3f n = (p - this->get_center()) * (1 / this->get_radius());
    return n;
}


/* Plane */

// Config
PlaneConfig::PlaneConfig(Vec3f origin, Vec3f normal): origin(origin), normal(normal.normalize()) {}

// getters
Vec3f Plane::get_origin(void) const { return Vec3f(this->read(1), this->read(2), this->read(3)); }
Vec3f Plane::get_normal(void) const { return Vec3f(this->read(4), this->read(5), this->read(6)); }
// setters
void Plane::set_origin(Vec3f o) { this->write(1, o.x()); this->write(2, o.y()); this->write(3, o.z()); }
void Plane::set_normal(Vec3f n) { this->write(4, n.x()); this->write(5, n.y()); this->write(6, n.z()); }

// apply config
void Plane::apply(Config* config) {
    // convert config
    PlaneConfig* config_ = (PlaneConfig*)config;
    Vec3f normal = config_->normal;
    // apply values from configuration
    this->set_origin(config_->origin);
    this->set_normal(config_->normal);
}

// ray-cast method
bool Plane::cast(const Vec3f origin, const Vec3f dir, float* t) const {
    // get normal and check if plane and ray are aligned
    Vec3f normal = this->get_normal();
    float denom = Vec3f::dot(normal, dir); 
    if (denom < -EPS) { 
        // compute distance to intersection point
        Vec3f p = this->get_origin() - origin; 
        *t = Vec3f::dot(p, normal) / denom;
        return (*t >= EPS);
    } 
    // no intersection
    return false; 
}


// normal of plane
Vec3f Plane::normal(Vec3f p) const { return this->get_normal(); }