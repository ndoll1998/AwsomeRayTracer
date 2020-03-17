#include "include/_defines.h"
#include "src/kernels/structs.cl"
#include "src/kernels/utils.cl"

/*** Diffuse Material ***/

void diffusematerial_apply(__global float* data, DiffuseMaterial* mat) {
    // read data into material
    mat->attenuation = (float3)(data[0], data[1], data[2]);
    mat->diffuse = data[3]; mat->specular = data[4]; mat->shininess = data[5];
}

float diffusematerial_get_diffuse(float3 p, Material* material, Globals* globals) {
    // create color-material from data
    DiffuseMaterial color_material; diffusematerial_apply(material->data, &color_material);
    // return color of material
    return color_material.diffuse;
}

float diffusematerial_get_specular(float3 p, Material* material, Globals* globals) {
    // create color-material from data
    DiffuseMaterial color_material; diffusematerial_apply(material->data, &color_material);
    // return color of material
    return color_material.specular;
}

float diffusematerial_get_shininess(float3 p, Material* material, Globals* globals) {
    // create color-material from data
    DiffuseMaterial color_material; diffusematerial_apply(material->data, &color_material);
    // return color of material
    return color_material.shininess;
}

float3 diffusematerial_get_attenuation(float3 p, float3 v, float3 n, Material* material, Globals* globals) {
    // create color-material from data
    DiffuseMaterial color_material; diffusematerial_apply(material->data, &color_material);
    // return color of material
    return color_material.attenuation;
}

int diffusematerial_get_scatter_ray(float3 p, float3 v, float3 n, Material* material, Ray* ray, Globals* globals) {
    // write scattered ray
    ray->origin = p;
    ray->direction = normalize(n + rand_in_unit_sphere(globals));
    // return if material scatters a ray at all
    return 1;
}


/*** Metal Material ***/

void metalmaterial_apply(__global float* data, MetalMaterial* mat) {
    // apply base material
    diffusematerial_apply(data, mat);
    // apply fuzzyness value
    mat->fuzz = data[6];
}

float metalmaterial_get_diffuse(float3 p, Material* material, Globals* globals) {
    // from base material
    return diffusematerial_get_diffuse(p, material, globals);
}

float metalmaterial_get_specular(float3 p, Material* material, Globals* globals) {
    // from base material
    return diffusematerial_get_specular(p, material, globals);
}

float metalmaterial_get_shininess(float3 p, Material* material, Globals* globals) {
    // from base material
    return diffusematerial_get_shininess(p, material, globals);
}

float3 metalmaterial_get_attenuation(float3 p, float3 v, float3 n, Material* material, Globals* globals) {
    // from base material
    return diffusematerial_get_attenuation(p, v, n, material, globals);
}

int metalmaterial_get_scatter_ray(float3 p, float3 v, float3 n, Material* material, Ray* ray, Globals* globals) {
    // get fuuzyness
    MetalMaterial mat; metalmaterial_apply(material->data, &mat);
    float fuzz = mat.fuzz;
    // write scattered ray
    ray->origin = p;
    ray->direction = normalize( -reflect(v, n) + fuzz * rand_in_unit_sphere(globals) );
    // return if ray is scattered
    return (dot(ray->direction, n) > 0);
}


/*** functions ***/

unsigned int material_get_type_size(unsigned int material_type_id) {
    // get type size of type given by type id
    switch (material_type_id) {
        case (MATERIAL_DIFFUSEMATERIAL_TYPE_ID):    return MATERIAL_DIFFUSEMATERIAL_TYPE_SIZE;
        case (MATERIAL_METALMATERIAL_TYPE_ID):      return MATERIAL_METALMATERIAL_TYPE_SIZE;
    }
}

float material_get_diffuse(
    // point of interest
    float3 p, 
    // material of interest
    Material* material,
    // globals
    Globals* globals
) {
    // get diffuse value
    switch (material->type_id) {
        case (MATERIAL_DIFFUSEMATERIAL_TYPE_ID):    return diffusematerial_get_diffuse(p, material, globals);
        case (MATERIAL_METALMATERIAL_TYPE_ID):      return metalmaterial_get_diffuse(p, material, globals);
    }
}

float material_get_specular(
    // point of interest
    float3 p, 
    // material of interest
    Material* material,
    // globals
    Globals* globals
) {
    // get specular value
    switch (material->type_id) {
        case (MATERIAL_DIFFUSEMATERIAL_TYPE_ID):    return diffusematerial_get_specular(p, material, globals);
        case (MATERIAL_METALMATERIAL_TYPE_ID):      return metalmaterial_get_specular(p, material, globals);
    }
}

float material_get_shininess(
    // point of interest
    float3 p, 
    // material of interest
    Material* material,
    // globals
    Globals* globals
) {
    // get shininess value
    switch (material->type_id) {
        case (MATERIAL_DIFFUSEMATERIAL_TYPE_ID):    return diffusematerial_get_shininess(p, material, globals);
        case (MATERIAL_METALMATERIAL_TYPE_ID):      return metalmaterial_get_shininess(p, material, globals);
    }
}

float3 material_get_attenuation(
    // point, view and normal of interest
    float3 p, float3 v, float3 n,
    // material of interest
    Material* material,
    // globals
    Globals* globals
) {
    // get attenuation value
    switch (material->type_id) {
        case (MATERIAL_DIFFUSEMATERIAL_TYPE_ID):    return diffusematerial_get_attenuation(p, v, n, material, globals);
        case (MATERIAL_METALMATERIAL_TYPE_ID):      return metalmaterial_get_attenuation(p, v, n, material, globals);
    }
}

int material_get_scatter_ray(
    // point, view and normal of interest
    float3 p, float3 v, float3 n,
    // material of interest
    Material* material,
    // scattered ray
    Ray* ray,
    // globals
    Globals* globals
) {
    // get scatter ray
    switch (material->type_id) {
        case (MATERIAL_DIFFUSEMATERIAL_TYPE_ID):    return diffusematerial_get_scatter_ray(p, v, n, material, ray, globals);
        case (MATERIAL_METALMATERIAL_TYPE_ID):      return metalmaterial_get_scatter_ray(p, v, n, material, ray, globals);
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
