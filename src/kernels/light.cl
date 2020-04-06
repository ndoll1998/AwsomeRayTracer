#pragma once
#include "include/_defines.h"
#include "src/kernels/structs.cl"
#include "src/kernels/ray.cl"
#include "src/kernels/utils.cl"

/*** Point Light ***/

float3 pointlight_get_position(Light* light) {
    // get position values of light
    return (float3)(light->data[3], light->data[4], light->data[5]);
}

float3 pointlight_direction(float3 p, Light* light, Globals* globals) {
    // return direction from p to light position
    return normalize(pointlight_get_position(light) - p);
}

float3 pointlight_color(float3 p, Light* light, Globals* globals) {
    // return color
    return (float3)(light->data[0], light->data[1], light->data[2]);
}

float pointlight_squarred_distance(float3 p, Light* light, Globals* globals) {
    // return squarred distance between point and light-source
    float3 dif = pointlight_get_position(light) - p;
    return dot(dif, dif);
}


/*** functions ***/

float3 light_get_direction(float3 p, Light* light, Globals* globals) {
    switch(light->type_id) {
        case (LIGHT_POINTLIGHT_TYPE_ID): return pointlight_direction(p, light, globals);
    }
}

float3 light_get_color(float3 p, Light* light, Globals* globals) {
    switch(light->type_id) {
        case (LIGHT_POINTLIGHT_TYPE_ID): return pointlight_color(p, light, globals);
    }
}

float light_get_squarred_distance(float3 p, Light* light, Globals* globals) {
    switch(light->type_id) {
        case (LIGHT_POINTLIGHT_TYPE_ID): return pointlight_squarred_distance(p, light, globals);
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
    Container* geometries,
    // ambient light color
    float3 ambient,
    // globals
    Globals* globals
) {
    float3 color = (float3)ambient;
    // current light
    Light l; l.data = lights->data;
    // loop over all lights
    for (unsigned int i = 0; i < lights->n; i++) {
        // set current light type
        l.type_id = lights->type_ids[i];
        // create ray from point towards light source
        float3 light_dir = light_get_direction(p, &l, globals);
        Ray r = (Ray){ p, light_dir };
        // check angle
        if (dot(light_dir, normal) <= EPS) continue;
        // check for objects between point and light-source
        Geometry closest; float t;
        if ( (!ray_cast_to_geometries(&r, geometries, &closest, &t, globals)) || (t*t > light_get_squarred_distance(p, &l, globals)) ) {
            // reflect light ray
            float3 light_reflect = reflect(light_dir, normal);
            // phong reflection model
            float diffuse = dot(light_dir, normal) * material_get_diffuse(p, material, globals);
            float specular = pow(-dot(light_reflect, vision_dir) * material_get_specular(p, material, globals), material_get_shininess(p, material, globals));
            // add all together
            color += light_get_color(p, &l, globals) * (diffuse + specular);
        }
        l.data += light_get_type_size(l.type_id);
    }
    // return resulting color
    return color;
}