#include "vec3f.hpp"
#include "memCompressor.hpp"
#include "_defines.h"

/* abstract base materials */

class Material : public Compressable {
    public:
    /* get color at position */
    virtual const Vec3f color(const Vec3f p) const = 0;
    /* phong reflection model values at position */
    virtual float diffuse(Vec3f p) const = 0;
    virtual float specular(Vec3f p) const = 0;
    virtual float shininess(Vec3f p) const = 0;
};


/* Color Material */

class ColorMaterialConfig : public Config {
    public:
    /* rgb, diffuse and specular values */
    float r, g, b, diff, spec, shiny;
    /* constructors */
    ColorMaterialConfig(float r, float g, float b, float diff, float spec, float shiny);
};

class ColorMaterial : public Material {
    private:
    /* getter and setter */
    Vec3f get_color(void) const;
    void set_color(float r, float g, float b);
    void set_phong(float diff, float spec, float shiny);

    public:
    /* Matrial Type ID and required size */
    unsigned int get_type_id(void) const { return MATERIAL_COLORMATERIAL_TYPE_ID; }
    unsigned int get_size(void) const { return MATERIAL_COLORMATERIAL_TYPE_SIZE; }
    /* apply config */
    void apply(Config* config);
    /* get color at given point */
    const Vec3f color(const Vec3f p) const;
    /* get phong reflection model values */
    float diffuse(Vec3f p) const;
    float specular(Vec3f p) const;
    float shininess(Vec3f p) const;
};
