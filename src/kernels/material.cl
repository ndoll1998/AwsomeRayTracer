#include "include/_defines.h"
#include "src/kernels/structs.cl"

/*** Color Material ***/

void colormaterial_apply(__global float* data, ColorMaterial* mat) {
    // read data into material
    mat->color = (float3)(data[0], data[1], data[2]);
    mat->diffuse = data[3]; mat->specular = data[4]; mat->shininess = data[5];
}

float3 colormaterial_get_color(float3 p, Material* material) {
    // create color-material from data
    ColorMaterial color_material; colormaterial_apply(material->data, &color_material);
    // return color of material
    return color_material.color;
}

float colormaterial_get_diffuse(float3 p, Material* material) {
    // create color-material from data
    ColorMaterial color_material; colormaterial_apply(material->data, &color_material);
    // return color of material
    return color_material.diffuse;
}

float colormaterial_get_specular(float3 p, Material* material) {
    // create color-material from data
    ColorMaterial color_material; colormaterial_apply(material->data, &color_material);
    // return color of material
    return color_material.specular;
}

float colormaterial_get_shininess(float3 p, Material* material) {
    // create color-material from data
    ColorMaterial color_material; colormaterial_apply(material->data, &color_material);
    // return color of material
    return color_material.shininess;
}


/*** functions ***/

unsigned int material_get_type_size(unsigned int material_type_id) {
    // get type size of type given by type id
    switch (material_type_id) {
        case (MATERIAL_COLORMATERIAL_TYPE_ID): return MATERIAL_COLORMATERIAL_TYPE_SIZE;
    }
}

void material_get(
    // target material id
    unsigned int target_material_id,
    // materials
    Container* materials,
    // resulting material
    Material* material
) {
    // make sure to stay in bounds
    if (target_material_id > materials->n) printf("MaterialID out of bounds!\n");
    // pointer to material data given by index
    __global float* target_material_data = materials->data;
    // go to index
    for (unsigned int i = 0; i < target_material_id; i++)
        target_material_data += material_get_type_size(materials->type_ids[i]);
    // set material
    material->data = target_material_data;
    material->type_id = materials->type_ids[target_material_id];
}

float3 material_get_base_color(
    // point of interest
    float3 p, 
    // material of interest
    Material* material
) {
    // get color
    switch (material->type_id) {
        case (MATERIAL_COLORMATERIAL_TYPE_ID): return colormaterial_get_color(p, material);
    }
}

float material_get_diffuse(
    // point of interest
    float3 p, 
    // material of interest
    Material* material
) {
    // get color
    switch (material->type_id) {
        case (MATERIAL_COLORMATERIAL_TYPE_ID): return colormaterial_get_diffuse(p, material);
    }
}

float material_get_specular(
    // point of interest
    float3 p, 
    // material of interest
    Material* material
) {
    // get color
    switch (material->type_id) {
        case (MATERIAL_COLORMATERIAL_TYPE_ID): return colormaterial_get_specular(p, material);
    }
}

float material_get_shininess(
    // point of interest
    float3 p, 
    // material of interest
    Material* material
) {
    // get color
    switch (material->type_id) {
        case (MATERIAL_COLORMATERIAL_TYPE_ID): return colormaterial_get_shininess(p, material);
    }
}
