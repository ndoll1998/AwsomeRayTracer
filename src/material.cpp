#include "material.hpp"
#include "math.h"

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

// config
MetalMaterialConfig::MetalMaterialConfig(float r, float g, float b, float diff, float spec, float shiny, float fuzzy): 
    DiffuseMaterialConfig(r, g, b, diff, spec, shiny), fuzzy(fuzzy) {}

// getter - setter
void MetalMaterial::set_fuzzy(float fuzzy) { this->write(6, fuzzy); }
float MetalMaterial::fuzzy(void) const { return this->read(6); }
// apply config
void MetalMaterial::apply(Config* config) { 
    // apply to base
    DiffuseMaterial::apply(config); 
    // convert and apply fuzzyness-value
    MetalMaterialConfig* metal_config = (MetalMaterialConfig*)config;
    this->set_fuzzy(metal_config->fuzzy); 
}
// scatter ray
bool MetalMaterial::scatter(Vec3f p, Vec3f v, Vec3f n, pair<Vec3f, Vec3f>* ray) const {
    // reflect vision ray at normal
    ray->first = p; 
    ray->second = ( v.reflect(n) * (-1) + Vec3f::rand_in_unit_sphere() * this->fuzzy() ).normalize();
    return (Vec3f::dot(ray->second, n) > 0);
}


/*** Dielectric Material ***/

// config
DielectricMaterialConfig::DielectricMaterialConfig(float diff, float spec, float shiny, float ior):
    diff(diff), spec(spec), shiny(shiny), ior(ior) {}

// setters
void DielectricMaterial::set_ior(float ior) { this->write(0, ior); }
void DielectricMaterial::set_phong(float diff, float spec, float shiny) { this->write(1, diff); this->write(2, spec); this->write(3, shiny); }
// getters
float DielectricMaterial::ior(void) const { return this->read(0); }
// public getters
float DielectricMaterial::diffuse(const Vec3f p) const { return this->read(1); }
float DielectricMaterial::specular(const Vec3f p) const { return this->read(2); }
float DielectricMaterial::shininess(const Vec3f p) const { return this->read(3); }
// apply config
void DielectricMaterial::apply(Config* config) {
    // convert config
    DielectricMaterialConfig* config_ = (DielectricMaterialConfig*)config;
    // apply values
    this->set_phong(config_->diff, config_->spec, config_->shiny);
    this->set_ior(config_->ior);
}


float schlick_approximation(float cosine, float ior) {
    // approximate probability of ray being reflected instead of refracted
    float r0 = (1.0f - ior) / (1.0f + ior);
    r0 = r0 * r0;
    return r0 + (1.0f - r0) * pow(1.0f - cosine, 5);
}

// attenuation and scattered ray
Vec3f DielectricMaterial::attenuation(Vec3f p, Vec3f v, Vec3f n) const { return Vec3f(1.0, 1.0, 1.0); }
bool DielectricMaterial::scatter(Vec3f p, Vec3f v, Vec3f n, pair<Vec3f, Vec3f>* ray) const {
    // set origin of scattered ray
    ray->first = p;

    Vec3f refracted; bool valid;
    // schlick approximation
    float cosine;
    // get refraction 
    if (Vec3f::dot(v, n) > 0.0f) {
        // leaving material
        valid = v.refract(n * (-1), this->ior(), &refracted);
        ray->first = ray->first + n * (5 * EPS);    // move origin out of geometry
        // get cosine angle for schlick approximation
        cosine = Vec3f::dot(v, n);
    } else {
        // entering material
        valid = v.refract(n, 1.0f / this->ior(), &refracted);
        ray->first = ray->first - n * (5 * EPS);    // move origin in geometry
        // get cosine angle for schlick approximation
        cosine = -Vec3f::dot(v, n);
    }

    // fresnel
    if (valid) {
        // approximate reflection probability
        float ref_prob = schlick_approximation(cosine, this->ior());
        // update valid based on probability
        valid = ((float)rand() / RAND_MAX) > ref_prob;
    }
    // either refract of reflect
    ray->second = valid? refracted : (v.reflect(n) * -1);
    return true;
}
