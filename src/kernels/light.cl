#pragma once
#include "include/_defines.h"
#include "src/kernels/structs.cl"
#include "src/kernels/ray.cl"
#include "src/kernels/utils.cl"

/*** Point Light ***/

void pointlight_apply(__global float* data, PointLight* light) {
    // read data into light struct
    light->color = (float3)(data[0], data[1], data[2]);
    light->position = (float3)(data[3], data[4], data[5]);
}

float3 pointlight_direction(float3 p, Light* light) {
    // create pointlight from light
    PointLight plight; pointlight_apply(light->data, &plight);
    // return direction from p to light position
    return normalize(plight.position - p);
}

float3 pointlight_color(float3 p, Light* light) {
    // create pointlight from light
    PointLight plight; pointlight_apply(light->data, &plight);
    // return color
    return plight.color;
}


/*** functions ***/

float3 light_get_direction(float3 p, Light* light) {
    switch(light->type_id) {
        case (LIGHT_POINTLIGHT_TYPE_ID): return pointlight_direction(p, light);
    }
}

float3 light_get_color(float3 p, Light* light) {
    switch(light->type_id) {
        case (LIGHT_POINTLIGHT_TYPE_ID): return pointlight_color(p, light);
    }
}

unsigned int light_get_type_size(unsigned int light_type) {
    switch (light_type) {
        case (LIGHT_POINTLIGHT_TYPE_ID): return LIGHT_POINTLIGHT_TYPE_SIZE;
    }
}

float3 light_get_total_light(
    // point of interest
    float3 p,
    // direction of vision towards point p
    float3 vision_dir,
    // normal on reflecting object at point p
    float3 normal,
    // material of reflecting object
    Material* material,
    // lights in scene
    Container* lights,
    // geometries in scene
    Container* geometries
) {
    float3 color = (0, 0, 0);
    // current light
    Light l; l.data = lights->data;
    // loop over all lights
    for (unsigned int i = 0; i < lights->n; i++) {
        // set current light type
        l.type_id = lights->type_ids[i];
        // create ray from point towards light source
        float3 light_dir = light_get_direction(p, &l);
        Ray r = (Ray){ p, light_dir };
        // check for objects between point and light-source
        Geometry closest; float t;
        if ((!ray_cast_to_geometries(&r, geometries, &closest, &t)) || (t < 0)) {
            // reflect light ray
            float3 light_reflect = reflect(light_dir, normal);
            // phong reflection model
            float diffuse = dot(light_dir, normal) * material_get_diffuse(p, material);
            float specular = pow(-dot(light_reflect, vision_dir) * material_get_specular(p, material), material_get_shininess(p, material));
            // add all together
            color += light_get_color(p, &l) * (diffuse + specular);
        }
        l.data += light_get_type_size(l.type_id);
    }
    // return resulting color
    return color;
}