#include "vec3f.hpp"
#include "memCompressor.hpp"
#include "_defines.h"

/* abstract base materials */

class Material : public Compressable {
    public:
    /* sactter direction and attenuation */
    virtual bool scatter(Vec3f p, Vec3f v, Vec3f n, pair<Vec3f, Vec3f>* ray) const = 0;
    virtual Vec3f attenuation(Vec3f p, Vec3f v, Vec3f n) const = 0;
    /* phong reflection model values at position */
    virtual float diffuse(Vec3f p) const = 0;
    virtual float specular(Vec3f p) const = 0;
    virtual float shininess(Vec3f p) const = 0;
};


/* Diffusion Material */

class DiffuseMaterialConfig : public Config {
    public:
    /* rgb, phong and absorb values */
    float r, g, b, diff, spec, shiny;
    /* constructors */
    DiffuseMaterialConfig(float r, float g, float b, float diff, float spec, float shiny);
};

class DiffuseMaterial : public Material {
    private:
    /* getter and setter */
    Vec3f get_color(void) const;
    void set_color(float r, float g, float b);
    void set_phong(float diff, float spec, float shiny);

    public:
    /* Matrial Type ID and required size */
    unsigned int get_type_id(void) const { return MATERIAL_DIFFUSEMATERIAL_TYPE_ID; }
    unsigned int get_size(void) const { return MATERIAL_DIFFUSEMATERIAL_TYPE_SIZE; }
    /* apply config */
    void apply(Config* config);
    /* override abstract methods */
    bool scatter(Vec3f p, Vec3f v, Vec3f n, pair<Vec3f, Vec3f>* ray) const;
    Vec3f attenuation(Vec3f p, Vec3f v, Vec3f n) const;
    /* get phong reflection model values */
    float diffuse(Vec3f p) const;
    float specular(Vec3f p) const;
    float shininess(Vec3f p) const;
};


/* Metal Material */

class MetalMaterialConfig : public DiffuseMaterialConfig {
    public:
    float fuzzy;
    /* constructor */
    MetalMaterialConfig(float r, float g, float b, float diff, float spec, float shiny, float fuzzy): DiffuseMaterialConfig(r, g, b, diff, spec, shiny), fuzzy(fuzzy) {}
};

class MetalMaterial : public DiffuseMaterial {
    private:
    /* setter */
    void set_fuzzy(float fuzzy);
    float fuzzy(void) const;

    public:
    /* Matrial Type ID and required size */
    unsigned int get_type_id(void) const { return MATERIAL_METALMATERIAL_TYPE_ID; }
    unsigned int get_size(void) const { return MATERIAL_METALMATERIAL_TYPE_SIZE; }
    /* pverride apply function */
    void apply(Config* config);
    /* override scatter function */
    bool scatter(Vec3f p, Vec3f v, Vec3f n, pair<Vec3f, Vec3f>* ray) const;
};
