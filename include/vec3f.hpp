#ifndef vec3f
#define vec3f

#include <iostream>
using namespace std;

class Vec3f {
    private:
    /* coords */
    float x_, y_, z_;

    public:
    /* constructors */
    Vec3f(void);
    Vec3f(float x, float y, float z);
    /* static constructors */
    static Vec3f rand_in_unit_sphere(void);
    /*  */
    float sum(void) const;
    float magnitude(void) const;
    /*  */
    Vec3f normalize(void) const;
    Vec3f rotate(const Vec3f axis, const float angle) const;
    Vec3f reflect(const Vec3f axis) const;
    Vec3f clamp(float a, float b) const;
    /* vec-vec-operators */
    static Vec3f cross(const Vec3f a, const Vec3f b);
    static Vec3f mul(const Vec3f a, const Vec3f b);
    static Vec3f add(const Vec3f a, const Vec3f b);
    static Vec3f sub(const Vec3f a, const Vec3f b);
    static float dot(const Vec3f a, const Vec3f b);
    /* vec-float-operators */
    static Vec3f mul(const Vec3f a, const float b);
    static Vec3f add(const Vec3f a, const float b);
    static Vec3f sub(const Vec3f a, const float b);
    /* vec-vec-overrides */
    Vec3f operator*(const Vec3f other) const { return Vec3f::mul(*this, other); }
    Vec3f operator+(const Vec3f other) const { return Vec3f::add(*this, other); }
    Vec3f operator-(const Vec3f other) const { return Vec3f::sub(*this, other); }
    /* vec-float-overrides */
    Vec3f operator*(const float other) const { return Vec3f::mul(*this, other); }
    Vec3f operator+(const float other) const { return Vec3f::add(*this, other); }
    Vec3f operator-(const float other) const { return Vec3f::sub(*this, other); }
    /* getters */
    float x(void) const { return this->x_; };
    float y(void) const { return this->y_; };
    float z(void) const { return this->z_; };
    /* setters */
    void x(float x) { this->x_ = x; }
    void y(float y) { this->y_ = y; }
    void z(float z) { this->z_ = z; }
}; 

#endif
