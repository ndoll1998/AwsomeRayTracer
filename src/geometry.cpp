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
    // apply values from configuration
    this->set_origin(config_->origin);
    this->set_normal(config_->normal);
}

// ray-cast method
bool Plane::cast(const Vec3f origin, const Vec3f dir, float* t) const {
    // get normal and check if plane and ray are aligned
    Vec3f normal = this->normal(origin);
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
Vec3f Plane::normal(Vec3f p) const { 
    // get direction from p towards plane
    Vec3f u = this->get_origin() - p;
    // return normal facing towards given point
    Vec3f normal = this->get_normal(); 
    return (Vec3f::dot(u, normal) < 0)? normal : (normal * -1);
}


/* Plane */

// Config
TriangleConfig::TriangleConfig(Vec3f A, Vec3f B, Vec3f C): A(A), B(B), C(C) {}

// getters
Vec3f Triangle::get_A(void) const { return Vec3f(this->read(1), this->read(2), this->read(3)); }
Vec3f Triangle::get_B(void) const { return Vec3f(this->read(4), this->read(5), this->read(6)); }
Vec3f Triangle::get_C(void) const { return Vec3f(this->read(7), this->read(8), this->read(9)); }
// setters
void Triangle::set_A(Vec3f A) { this->write(1, A.x()); this->write(2, A.y()); this->write(3, A.z()); }
void Triangle::set_B(Vec3f B) { this->write(4, B.x()); this->write(5, B.y()); this->write(6, B.z()); }
void Triangle::set_C(Vec3f C) { this->write(7, C.x()); this->write(8, C.y()); this->write(9, C.z()); }

// apply config
void Triangle::apply(Config* config) {
    // convert config
    TriangleConfig* config_ = (TriangleConfig*)config;
    // apply values from configuration
    this->set_A(config_->A);
    this->set_B(config_->B);
    this->set_C(config_->C);
}

// ray-cast method
bool Triangle::cast(const Vec3f origin, const Vec3f dir, float* t) const {
    // check plane intersection
    if (!Plane::cast(origin, dir, t)) return false;
    // check if intersection point is in triangle
    Vec3f P = origin + dir * (*t);

    // check with all edges of the triangle
    Vec3f A = this->get_A();
    Vec3f B = this->get_B();
    Vec3f C = this->get_C();
    // get unnormalized normal pointing towards origin of ray
    Vec3f normal = Vec3f::cross(A - B, A - C);
    normal = normal * ((Vec3f::dot(A - origin, normal) < 0)? 1 : -1);
    // AB
    Vec3f AB_AP = Vec3f::cross(B - A, P - A);    
    if (Vec3f::dot(AB_AP, normal) > 0) return false;
    // BC
    Vec3f BC_BP = Vec3f::cross(C - B, P - B);    
    if (Vec3f::dot(BC_BP, normal) > 0) return false;
    // CA
    Vec3f CA_CP = Vec3f::cross(A - C, P - C);    
    if (Vec3f::dot(CA_CP, normal) > 0) return false;
    // P is inside triangle
    return true;
}


// normal of plane
Vec3f Triangle::normal(Vec3f p) const {
    Vec3f A = this->get_A();
    // get direction
    Vec3f u = A - p;
    // get spanning vectors
    Vec3f v = A - this->get_B();
    Vec3f w = A - this->get_C(); 
    // compute normal facing towards given point
    Vec3f normal = Vec3f::cross(v, w).normalize();
    return (Vec3f::dot(u, normal) < 0)? normal : (normal * -1);
}