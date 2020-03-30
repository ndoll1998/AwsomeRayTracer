#include "light.hpp"


/*** Light ***/

// getters - setters
Vec3f Light::get_color(void) const { return Vec3f(this->read(0), this->read(1), this->read(2)); }
void Light::set_color(float r, float g, float b) { this->write(0, r); this->write(1, g); this->write(2, b); }


/*** Point Light ***/

// Config
PointLightConfig::PointLightConfig(float x, float y, float z, float r, float g, float b): x(x), y(y), z(z), r(r), g(g), b(b) {}

// getters - setters
Vec3f PointLight::get_position(void) const { return Vec3f(this->read(3), this->read(4), this->read(5)); }
void PointLight::set_position(float x, float y, float z) { this->write(3, x); this->write(4, y); this->write(5, z); }

// apply config
void PointLight::apply(Config* config) {
    // convert config
    PointLightConfig* config_ = (PointLightConfig*) config;
    // apply values from configuration
    this->set_position(config_->x, config_->y, config_->z);
    this->set_color(config_->r, config_->g, config_->b);
}

// light direction
Vec3f PointLight::light_direction(Vec3f p) const { return (this->get_position() - p).normalize(); }
// light distance
float PointLight::light_distance_squarred(Vec3f p) const { return Vec3f::dot(this->get_position() - p, this->get_position() - p); }
// color at position
Vec3f PointLight::light_color(Vec3f p) const { return this->get_color(); }