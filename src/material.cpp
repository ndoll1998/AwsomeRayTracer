#include "material.hpp"

#include <iostream>
using namespace std;

/*** Color ***/

Color::Color(unsigned char r, unsigned char g, unsigned char b): r_(r), g_(g), b_(b) {}
Color Color::scale(const float t) const { 
    return Color(
        (unsigned char)(this->r() * t), 
        (unsigned char)(this->g() * t), 
        (unsigned char)(this->b() * t)
    ); 
}
Color Color::add(const Color other) const { 
    return Color(this->r() + other.r(), this->g() + other.g(), this->b() + other.b()); 
}

/*** Base Material ***/

BaseMaterial::BaseMaterial(int r, int g, int b): color_(r, g, b) {}
const Color BaseMaterial::color(Vec3f p) const { return this->color_; }
