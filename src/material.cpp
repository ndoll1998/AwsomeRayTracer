#include "material.hpp"

/*** Color Material ***/

// Config
DiffuseMaterialConfig::DiffuseMaterialConfig(float r, float g, float b, float diff, float spec, float shiny, float absorb): 
    r(r), g(g), b(b), diff(diff), spec(spec), shiny(shiny), absorb(absorb) {}

// private members
Vec3f DiffuseMaterial::get_color(void) const { return Vec3f(this->read(0), this->read(1), this->read(2)); }
void DiffuseMaterial::set_color(float r, float g, float b) { this->write(0, r); this->write(1, g); this->write(2, b); }
void DiffuseMaterial::set_phong(float diff, float spec, float shiny) { this->write(3, diff); this->write(4, spec); this->write(5, shiny); }
void DiffuseMaterial::set_absorb(float absorb) { this->write(6, absorb); }
// public getters
const Vec3f DiffuseMaterial::color(const Vec3f p) const { return this->get_color(); }
float DiffuseMaterial::diffuse(const Vec3f p) const { return this->read(3); }
float DiffuseMaterial::specular(const Vec3f p) const { return this->read(4); }
float DiffuseMaterial::shininess(const Vec3f p) const { return this->read(5); }
// override abstract methods
float DiffuseMaterial::attenuation(Vec3f p, Vec3f v, Vec3f n) const { return this->read(6); }
pair<Vec3f, Vec3f> DiffuseMaterial::scatter(Vec3f p, Vec3f v, Vec3f n) const { 
    // build scatter ray
    return make_pair(p, ( n + Vec3f::rand_in_unit_circle() ).normalize());
}

// apply config to material
void DiffuseMaterial::apply(Config* config) { 
    // convert config and apply values
    DiffuseMaterialConfig* config_ = (DiffuseMaterialConfig*)config;
    this->set_color(config_->r, config_->g, config_->b); 
    this->set_phong(config_->diff, config_->spec, config_->shiny);
    this->set_absorb(config_->absorb);
}
