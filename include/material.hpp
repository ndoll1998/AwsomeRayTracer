#include "vec3f.hpp"

/*** Color Class ***/

class Color {
    private:
    /* color values */
    unsigned char r_, g_, b_;
    public:
    /* constructors */
    Color(unsigned char r, unsigned char g, unsigned char b);
    /* methods */
    Color scale(float t) const;
    Color add(const Color other) const;
    /* operators */
    Color operator*(const float t) const { return this->scale(t); }
    Color operator*(const Color other) const { return this->add(other); }
    /* getters */
    unsigned char r(void) const { return this->r_; }
    unsigned char g(void) const { return this->g_; }
    unsigned char b(void) const { return this->b_; }
};


/*** Materials ***/

class BaseMaterial {

    private:
    /* color */
    Color color_;

    public:
    /* constructors */
    BaseMaterial(int r, int g, int b);

    /* get color at position */
    virtual const Color color(const Vec3f p) const;
};
