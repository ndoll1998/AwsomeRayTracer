#pragma once
#include "Vec3f.hpp"
#include "memCompressor.hpp"
#include "_defines.h"

// abstract light class

class Light : public Compressable {
    protected:
    /* color setter */
    Vec3f get_color(void) const;
    void set_color(float r, float g, float b);

    public:
    /* abstract methods */
    /* direction toward light source from given point */
    virtual Vec3f light_direction(Vec3f p) const = 0;
    /* distance between point and light source */
    virtual float light_distance_squarred(Vec3f p) const = 0;
    /* light color at position p */
    virtual Vec3f light_color(Vec3f p) const = 0;
};


// Point Light

class PointLightConfig : public Config {
    public:
    /* light position and color values */
    float x, y, z;
    float r, g, b;
    /* constructor */
    PointLightConfig(float x, float y, float z, float r, float g, float b);
};

class PointLight : public Light {

    private:
    /* getters - setters */
    Vec3f get_position(void) const;
    void set_position(float x, float y, float z);

    public:
    /* Light Type ID and required size */
    unsigned int get_type_id(void) const { return LIGHT_POINTLIGHT_TYPE_ID; }
    unsigned int get_size(void) const { return LIGHT_POINTLIGHT_TYPE_SIZE; }
    /* apply config */
    void apply(Config* config);
    /* override light methods */
    Vec3f light_direction(Vec3f p) const;
    float light_distance_squarred(Vec3f p) const;
    Vec3f light_color(Vec3f p) const;
};
