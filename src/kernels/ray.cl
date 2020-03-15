#pragma once
#include "src/kernels/structs.cl"
#include "src/kernels/geometry.cl"

float3 advance_ray(Ray* ray, float t) {
    // compute point on ray at distance t
    return ray->origin + ray->direction * t;
}

int cast_ray(Ray* ray,
    // geometries
    __global float*        geometry_data,
    __global unsigned int* geometry_ids,
    unsigned int           n_geometries,
    // return geometry
    __global float** closest_ptr,
    unsigned int* closest_geometry,
    // return distance
    float* t
) {
    // find first intersecting geometry
    float t_cur; int hit = 0;
    // ppinter to head of data of current geometry
    __global float* cur_data_ptr = geometry_data;
    // loop throu all geometries
    for (unsigned int i = 0; i < n_geometries; i++) {
        // get geometry type
        unsigned int geo_type = geometry_ids[i];
        // cast ray to geometry
        if (cast_geometry(ray, geo_type, cur_data_ptr, &t_cur)) {
            // update clostest
            if ((t_cur < *t) || (!hit)) { 
                *closest_ptr = cur_data_ptr;
                *closest_geometry = i; 
                *t = t_cur; 
            }
            // set hit
            hit = 1;
        }
        // increase pointer to next geometry
        cur_data_ptr += get_geometry_type_size(geo_type);
    }
    return hit;
}

float3 get_ray_color() {
    
}
