#include "include/_defines.h"
#include "src/kernels/structs.cl"

/*** Color Material ***/

void colormaterial_apply(__global float* data, ColorMaterial* mat) {
    // read data into material
    mat->color = (float3)(data[0], data[1], data[2]);
}

float3 colormaterial_get_color(float3 p, __global float* data) {
    // create color-material from data
    ColorMaterial material; colormaterial_apply(data, &material);
    // return color of material
    return material.color;
}


/*** functions ***/

unsigned int get_material_type_size(unsigned int material_type_id) {
    // get type size of type given by type id
    switch (material_type_id) {
        case (MATERIAL_COLORMATERIAL_TYPE_ID): return MATERIAL_COLORMATERIAL_TYPE_SIZE;
    }
}

__global float* get_material(
    // material id
    unsigned int target_material_id,
    // materials
    __global float*        material_data, 
    __global unsigned int* material_ids, 
    unsigned int           n_materials
) {
    // make sure to stay in bounds
    if (target_material_id > n_materials) printf("MaterialID out of bounds");
    // pointer to material data given by index
    __global float* target_material_data = material_data;
    // go to index
    for (unsigned int i = 0; i < target_material_id; i++)
        target_material_data += get_material_type_size(material_ids[i]);
    // return pointer
    return target_material_data;
}

float3 get_material_color(
    // point of interest
    float3 p, 
    // material of interest
    unsigned int material_id,
    // materials
    __global float*        material_data, 
    __global unsigned int* material_ids, 
    unsigned int           n_materials
) {
    // get material
    __global float* material = get_material(material_id, material_data, material_ids, n_materials);
    // get color
    switch (material_ids[material_id]) {
        case (MATERIAL_COLORMATERIAL_TYPE_ID): return colormaterial_get_color(p, material);
    }

}
