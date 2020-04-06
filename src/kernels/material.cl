#include "include/_defines.h"
#include "src/kernels/structs.cl"
#include "src/kernels/utils.cl"

/*** Diffuse Material ***/

float diffusematerial_get_diffuse(float3 p, Material* material, Globals* globals) {
    // return diffuse value of material
    return material->data[3];
}

float diffusematerial_get_specular(float3 p, Material* material, Globals* globals) {
    // return specular value of material
    return material->data[4];
}

float diffusematerial_get_shininess(float3 p, Material* material, Globals* globals) {
    // return shininess value of material
    return material->data[5];
}

float3 diffusematerial_get_attenuation(float3 p, float3 v, float3 n, Material* material, Globals* globals) {
    // return attenuation of material
    return (float3)(material->data[0], material->data[1], material->data[2]);
}

int diffusematerial_get_scatter_ray(float3 p, float3 v, float3 n, Material* material, Ray* ray, Globals* globals) {
    // write scattered ray
    ray->origin = p;
    ray->direction = normalize(n + rand_in_unit_sphere(globals));
    // return if material scatters a ray at all
    return 1;
}


/*** Metal Material ***/

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
    // get fuzzyness
    float fuzz = material->data[6];
    // write scattered ray
    ray->origin = p;
    ray->direction = normalize( -reflect(v, n) + fuzz * rand_in_unit_sphere(globals) );
    // return if ray is scattered
    return (dot(ray->direction, n) > 0);
}


/*** Dielectric Material ***/

float dielectricmaterial_get_diffuse(float3 p, Material* material, Globals* globals) {
    // read diffuse value
    return material->data[1];
}

float dielectricmaterial_get_specular(float3 p, Material* material, Globals* globals) {
    // read specular value
    return material->data[2];
}

float dielectricmaterial_get_shininess(float3 p, Material* material, Globals* globals) {
    // read shininess value
    return material->data[3];
}

float3 dielectricmaterial_get_attenuation(float3 p, float3 v, float3 n, Material* material, Globals* globals) {
    // apply color of scattered ray
    return (float3)(1.0, 1.0, 1.0);
}

int dielectricmaterial_get_scatter_ray(float3 p, float3 v, float3 n, Material* material, Ray* ray, Globals* globals) {
    // get index of refraction of given material
    float ior = material->data[0];
    // set origin of scattered ray
    ray->origin = p;

    float3 refracted; int valid;
    // schlick approximation
    float cosine;
    // get refracted direction
    if (dot(v, n) > 0.0f) {
        // leaving material
        valid = refract(v, -n, ior, &refracted);
        ray->origin += n * (5.0f * EPS); // move origin inside geometry
        // get cosine angle for schlick approximation
        cosine = dot(v, n);
    } else {
        // entering material
        valid = refract(v, n, 1.0f / ior, &refracted);
        ray->origin -= n * (5.0f * EPS); // move origin outside geometry
        // get cosine angle for schlick approximation
        cosine = -dot(v, n);
    }

    // fresnel
    if (valid) {
        // schlick approximation
        float p = (1.0f - ior) / (1.0f + ior);
        p = p * p; p = p + (1.0f - p) * pow(1.0f - cosine, 5);
        // update valid based on probability
        valid = randf(globals) > p;
    }
    // either reflect or refract
    ray->direction = valid? refracted : -reflect(v, n);
    return 1;
}


/*** functions ***/

unsigned int material_get_type_size(unsigned int material_type_id) {
    // get type size of type given by type id
    switch (material_type_id) {
        case (MATERIAL_DIFFUSE_TYPE_ID):    return MATERIAL_DIFFUSE_TYPE_SIZE;
        case (MATERIAL_METAL_TYPE_ID):      return MATERIAL_METAL_TYPE_SIZE;
        case (MATERIAL_DIELECTRIC_TYPE_ID):      return MATERIAL_DIELECTRIC_TYPE_SIZE;
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
        case (MATERIAL_DIFFUSE_TYPE_ID):    return diffusematerial_get_diffuse(p, material, globals);
        case (MATERIAL_METAL_TYPE_ID):      return metalmaterial_get_diffuse(p, material, globals);
        case (MATERIAL_DIELECTRIC_TYPE_ID): return dielectricmaterial_get_diffuse(p, material, globals);
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
        case (MATERIAL_DIFFUSE_TYPE_ID):    return diffusematerial_get_specular(p, material, globals);
        case (MATERIAL_METAL_TYPE_ID):      return metalmaterial_get_specular(p, material, globals);
        case (MATERIAL_DIELECTRIC_TYPE_ID): return dielectricmaterial_get_specular(p, material, globals);
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
        case (MATERIAL_DIFFUSE_TYPE_ID):    return diffusematerial_get_shininess(p, material, globals);
        case (MATERIAL_METAL_TYPE_ID):      return metalmaterial_get_shininess(p, material, globals);
        case (MATERIAL_DIELECTRIC_TYPE_ID): return dielectricmaterial_get_shininess(p, material, globals);
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
        case (MATERIAL_DIFFUSE_TYPE_ID):    return diffusematerial_get_attenuation(p, v, n, material, globals);
        case (MATERIAL_METAL_TYPE_ID):      return metalmaterial_get_attenuation(p, v, n, material, globals);
        case (MATERIAL_DIELECTRIC_TYPE_ID): return dielectricmaterial_get_attenuation(p, v, n, material, globals);
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
        case (MATERIAL_DIFFUSE_TYPE_ID):    return diffusematerial_get_scatter_ray(p, v, n, material, ray, globals);
        case (MATERIAL_METAL_TYPE_ID):      return metalmaterial_get_scatter_ray(p, v, n, material, ray, globals);
        case (MATERIAL_DIELECTRIC_TYPE_ID): return dielectricmaterial_get_scatter_ray(p, v, n, material, ray, globals);
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
    __local float* target_material_data = materials->data;
    // go to index
    for (unsigned int i = 0; i < target_material_id; i++)
        target_material_data += material_get_type_size(materials->type_ids[i]);
    // set material
    material->data = target_material_data;
    material->type_id = materials->type_ids[target_material_id];
}
