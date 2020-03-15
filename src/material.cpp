#include "material.hpp"

/*** Color Material ***/

// Config
ColorMaterialConfig::ColorMaterialConfig(float r, float g, float b, float diff, float spec, float shiny, float reflect): 
    r(r), g(g), b(b), diff(diff), spec(spec), shiny(shiny), refl(reflect) {}

// private members
Vec3f ColorMaterial::get_color(void) const { return Vec3f(this->read(0), this->read(1), this->read(2)); }
void ColorMaterial::set_color(float r, float g, float b) { this->write(0, r); this->write(1, g); this->write(2, b); }
void ColorMaterial::set_phong(float diff, float spec, float shiny) { this->write(3, diff); this->write(4, spec); this->write(5, shiny); }
void ColorMaterial::set_reflection(float reflection) { this->write(6, reflection); }
// public getters
const Vec3f ColorMaterial::color(const Vec3f p) const { return this->get_color(); }
float ColorMaterial::diffuse(const Vec3f p) const { return this->read(3); }
float ColorMaterial::specular(const Vec3f p) const { return this->read(4); }
float ColorMaterial::shininess(const Vec3f p) const { return this->read(5); }
float ColorMaterial::reflection(const Vec3f p) const { return this->read(6); }
// apply config to material
void ColorMaterial::apply(Config* config) { 
    // convert config and apply values
    ColorMaterialConfig* config_ = (ColorMaterialConfig*)config;
    this->set_color(config_->r, config_->g, config_->b); 
    this->set_phong(config_->diff, config_->spec, config_->shiny);
    this->set_reflection(config_->refl);
}
