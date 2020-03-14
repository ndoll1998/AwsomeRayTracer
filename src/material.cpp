#include "material.hpp"

/*** Color Material ***/

// Config
ColorMaterialConfig::ColorMaterialConfig(float r, float g, float b): r(r), g(g), b(b) {}

// private members
Vec3f ColorMaterial::get_color(void) const { return Vec3f(this->read(0), this->read(1), this->read(2)); }
void ColorMaterial::set_color(float r, float g, float b) { this->write(0, r); this->write(1, g); this->write(2, b); }
// public members
const Vec3f ColorMaterial::color(const Vec3f p) const { return this->get_color(); }
void ColorMaterial::apply(Config* config) { 
    // convert config and apply values
    ColorMaterialConfig* config_ = (ColorMaterialConfig*)config;
    this->set_color(config_->r, config_->g, config_->b); 
}
