#pragma once
#include "src/kernels/structs.cl"
#include "src/kernels/geometry.cl"
#include "include/_defines.h"

float3 ray_advance(Ray* ray, float t) {
    // compute point on ray at distance t
    return ray->origin + ray->direction * t;
}

int ray_cast_to_geometries(
    Ray* ray,
    // geometries
    Container* geometries,
    // return geometry and distance
    Geometry* closest, float* t,
    // globals
    Globals* globals
) {
    // find closest intersecting geometry
    Geometry geometry; geometry.data = geometries->data;
    float t_cur; int hit = 0;
    // loop throu all geometries
    for (unsigned int i = 0; i < geometries->n; i++) {
        // set current geometry type
        geometry.type_id = geometries->type_ids[i];
        // cast ray to geometry
        if (geometry_cast_ray(ray, &geometry, &t_cur, globals)) {
            // update closest
            if ((t_cur < *t - EPS) || (!hit)) { 
                closest->data = geometry.data;
                closest->type_id = geometry.type_id;
                *t = t_cur; 
            }
            // set hit
            hit = 1;
        }
        // increase pointer to next geometry
        geometry.data += geometry_get_type_size(geometry.type_id);
    }
    return hit;
}
