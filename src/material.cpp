#include "material.hpp"

/*** Color Material ***/

// Config
DiffuseMaterialConfig::DiffuseMaterialConfig(float r, float g, float b, float diff, float spec, float shiny): 
    r(r), g(g), b(b), diff(diff), spec(spec), shiny(shiny) {}

// private members
Vec3f DiffuseMaterial::get_color(void) const { return Vec3f(this->read(0), this->read(1), this->read(2)); }
void DiffuseMaterial::set_color(float r, float g, float b) { this->write(0, r); this->write(1, g); this->write(2, b); }
void DiffuseMaterial::set_phong(float diff, float spec, float shiny) { this->write(3, diff); this->write(4, spec); this->write(5, shiny); }
// public getters
float DiffuseMaterial::diffuse(const Vec3f p) const { return this->read(3); }
float DiffuseMaterial::specular(const Vec3f p) const { return this->read(4); }
float DiffuseMaterial::shininess(const Vec3f p) const { return this->read(5); }
// override abstract methods
Vec3f DiffuseMaterial::attenuation(Vec3f p, Vec3f v, Vec3f n) const { return this->get_color(); }
bool DiffuseMaterial::scatter(Vec3f p, Vec3f v, Vec3f n, pair<Vec3f, Vec3f>* ray) const { 
    // build scatter ray
    ray->first = p; ray->second = ( n + Vec3f::rand_in_unit_sphere() ).normalize();
    return true;
}

// apply config to material
void DiffuseMaterial::apply(Config* config) { 
    // convert config and apply values
    DiffuseMaterialConfig* config_ = (DiffuseMaterialConfig*)config;
    this->set_color(config_->r, config_->g, config_->b); 
    this->set_phong(config_->diff, config_->spec, config_->shiny);
}


/*** Metal Material ***/

/* getter - setter */
void MetalMaterial::set_fuzzy(float fuzzy) { this->write(6, fuzzy); }
float MetalMaterial::fuzzy(void) const { return this->read(6); }
/* apply config */
void MetalMaterial::apply(Config* config) { 
    DiffuseMaterial::apply(config); 
    MetalMaterialConfig* metal_config = (MetalMaterialConfig*)config;
    this->set_fuzzy(metal_config->fuzzy); 
}
/* scatter ray */
bool MetalMaterial::scatter(Vec3f p, Vec3f v, Vec3f n, pair<Vec3f, Vec3f>* ray) const {
    // reflect vision ray at normal
    ray->first = p; 
    ray->second = ( v.reflect(n) * (-1) + Vec3f::rand_in_unit_sphere() * this->fuzzy() ).normalize();
    return (Vec3f::dot(ray->second, n) > 0);
}