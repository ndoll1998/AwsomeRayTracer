#include "vec3f.hpp"
#include <math.h>

/*** constructors ***/

Vec3f::Vec3f(void): x_(0), y_(0), z_(0) {}
Vec3f::Vec3f(float x, float y, float z): x_(x), y_(y), z_(z) {}

/*** static constructors ***/

Vec3f Vec3f::rand_in_unit_circle(void) {
    // create random numbers betweem -1 and 1
    float x = 2 * ((float) std::rand() / RAND_MAX) - 1;
    float y = 2 * ((float) std::rand() / RAND_MAX) - 1;
    float z = 2 * ((float) std::rand() / RAND_MAX) - 1;
    // fill vector
    Vec3f p(x, y, z);
    
    // check if p is in unit circle
    if (Vec3f::dot(p, p) < 1) return p;
    // else scale it to be in unit circle
    float m = (float) std::rand() / RAND_MAX;
    return p.normalize() * m;
}

/*** public methods ***/

float Vec3f::sum(void) const {
    // sum all elements
    return this->x() + this->y() + this->z();
}

float Vec3f::magnitude(void) const {
    // length of vector
    return sqrtf(Vec3f::dot(*this, *this));
}

Vec3f Vec3f::normalize(void) const {
    // compute norm
    float norm_squarred = Vec3f::dot(*this, *this);
    // already normalized
    if (abs(norm_squarred - 1) > 1e-5) {
        // compute norm - avoid division by zero
        float scale = 1 / (sqrtf(norm_squarred) + 1e-5);
        // normalize all elements
        return Vec3f::mul(*this, scale);
    }
    return Vec3f(*this);
}

Vec3f Vec3f::rotate(const Vec3f axis, const float theta) const {
    // normalize
    Vec3f v = this->normalize();
    Vec3f a = axis.normalize();
    // compute values
    float c = cos(theta), s = sin(theta);
    float d = Vec3f::dot(a, v);
    Vec3f w = Vec3f::cross(a, v);
    // Rodrigues' rotation formula
    return v * c + w * s + a * d * (1 - c);
}

Vec3f Vec3f::reflect(Vec3f axis) const {
    // normalize
    Vec3f v = this->normalize();
    Vec3f a = axis.normalize();
    // reflect v over axis a
    return a * (Vec3f::dot(v, a) * 2) - v;
}

Vec3f Vec3f::clamp(float a, float b) const {
    // create vector with clipped values
    return Vec3f(min(max(a, this->x()), b), min(max(a, this->y()), b), min(max(a, this->z()), b));
}

/*** vec-vec-operators ***/

Vec3f Vec3f::cross(const Vec3f a, const Vec3f b) {
    // cross-product of two vectors
    return Vec3f(a.y() * b.z() - a.z() * b.y(), a.z() * b.x() - a.x() * b.z(), a.x() * b.y() - a.y() * b.x() );
}

Vec3f Vec3f::mul(const Vec3f a, const Vec3f b) {
    // elementwise multiplication
    return Vec3f( a.x() * b.x(), a.y() * b.y(), a.z() * b.z() );
}

Vec3f Vec3f::add(const Vec3f a, const Vec3f b) {
    // elementwise addition
    return Vec3f( a.x() + b.x(), a.y() + b.y(), a.z() + b.z() );
}

Vec3f Vec3f::sub(const Vec3f a, const Vec3f b) {
    // elementwise subtraction
    return Vec3f( a.x() - b.x(), a.y() - b.y(), a.z() - b.z() );
}

float Vec3f::dot(const Vec3f a, const Vec3f b) {
    // dot product
    return Vec3f::mul(a, b).sum();
}


/*** vec-float-operators ***/

Vec3f Vec3f::mul(const Vec3f a, const float b) {
    // scaling vector
    return Vec3f( a.x() * b, a.y() * b, a.z() * b );
}

Vec3f Vec3f::add(const Vec3f a, const float b) {
    // adding
    return Vec3f( a.x() + b, a.y() + b, a.z() + b );
}

Vec3f Vec3f::sub(const Vec3f a, const float b) {
    // subtracting
    return Vec3f( a.x() - b, a.y() - b, a.z() - b );
}



