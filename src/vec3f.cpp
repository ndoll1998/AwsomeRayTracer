#include "vec3f.hpp"
#include <math.h>

/*** constructors ***/

Vec3f::Vec3f(void): x_(0), y_(0), z_(0) {}
Vec3f::Vec3f(float x, float y, float z): x_(x), y_(y), z_(z) {}

/*** public methods ***/

float Vec3f::sum(void) const {
    // sum all elements
    return this->x() + this->y() + this->z();
}

float Vec3f::magnitude(void) const {
    // length of vector
    return sqrtf(Vec3f::dot(*this, *this));
}

void Vec3f::normalize(void) {
    // compute norm
    float norm_squarred = Vec3f::dot(*this, *this);
    // already normalized
    if (abs(norm_squarred - 1) > 1e-5) {
        // compute norm - avoid division by zero
        float scale = 1 / (sqrtf(norm_squarred) + 1e-5);
        // normalize all elements
        this->x(this->x() * scale);
        this->y(this->y() * scale);
        this->z(this->z() * scale);
    }
}

Vec3f Vec3f::rotate(const Vec3f axis, const float theta) const {
    Vec3f norm_axis;
    //normalize axis if necessary
    float norm_squarred = Vec3f::dot(*this, *this);
    // check if axis is already normalized
    if (abs(norm_squarred - 1) > 1e-5) {
        // normalize axis
        float scale = 1 / (sqrtf(norm_squarred) + 1e-5);
        norm_axis = axis * scale;
    } else { norm_axis = axis; }
    // compute values
    float c = cos(theta), s = sin(theta);
    float d = Vec3f::dot(norm_axis, *this);
    Vec3f w = Vec3f::cross(norm_axis, *this);
    // Rodrigues' rotation formula
    return (*this) * c + w * s + norm_axis * d * (1 - c);
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



