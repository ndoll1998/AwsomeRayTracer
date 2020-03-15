#include "vec3f.hpp"
#include "memCompressor.hpp"
#include "_defines.h"

/* abstract base materials */

class BaseMaterial : public Compressable {
    public:
    /* get color at position */
    virtual const Vec3f color(const Vec3f p) const = 0;
};


/* Color Material */

class ColorMaterialConfig : public Config {
    public:
    /* rgb values */
    float r, g, b;
    /* constructors */
    ColorMaterialConfig(float r, float g, float b);
};

class ColorMaterial : public BaseMaterial {
    private:
    /* getter and setter */
    Vec3f get_color(void) const;
    void set_color(float r, float g, float b);

    public:
    /* Matrial Type ID and required size */
    unsigned int get_type_id(void) const { return MATERIAL_COLORMATERIAL_TYPE_ID; }
    unsigned int get_size(void) const { return MATERIAL_COLORMATERIAL_TYPE_SIZE; }
    /* apply config */
    void apply(Config* config);
    /* get color at given point */
    const Vec3f color(const Vec3f p) const;
};
