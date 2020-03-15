#pragma once
#include "include/_defines.h"
#include "src/kernels/structs.cl"

/*** Sphere ***/

void sphere_apply(__global float* data, Sphere* sphere) {
    // read data into sphere
    sphere->center = (float3)(data[1], data[2], data[3]);
    sphere->radius = data[4];
}

int sphere_cast(Ray *ray, __global float* data, float* t) {
    // create sphere from data
    Sphere s; sphere_apply(data, &s);
    // analytic solution of sphere-ray-intersection
    float3 L = ray->origin - s.center;
    float a = dot(ray->direction, ray->direction);
    float b = dot(ray->direction, L) * 2;
    float c = dot(L, L) - s.radius * s.radius;
    // solve quadratic function
    float discr = b * b - 4 * a * c;
    // no intersection
    if (discr < 0) return 0;
    // one intersection
    else if (discr == 0) { *t = -0.5 * b / a; }
    // two intersections
    else {
        float q = (b > 0)? 
            -0.5 * (b + sqrt(discr)):
            -0.5 * (b - sqrt(discr));
        *t = min(q/ a, c / q);
    }
    return 1;
}

float3 sphere_normal(float3 p, __global float* data){
    // create sphere from data
    Sphere s; sphere_apply(data, &s);
    // compute normal at position p on surface
    return normalize(p - s.center);
}


/*** functions ***/

unsigned int get_geometry_type_size(unsigned int geometry_type) {
    // return size of type specified by type-id
    switch(geometry_type) {
        case (GEOMETRY_SPHERE_TYPE_ID): return GEOMETRY_SPHERE_TYPE_SIZE;
        default: break;
    }
}

int cast_geometry(Ray *ray, unsigned int geometry_type, __global float* geometry_data, float* t) {
    // cast to geometry specified by type-id
    switch(geometry_type) {
        case (GEOMETRY_SPHERE_TYPE_ID): return sphere_cast(ray, geometry_data, t);
    }
}

float3 get_normal(float3 p, unsigned int geometry_type, __global float* data) {
    // get normal on surface of geometry specified by type and data
    switch(geometry_type) {
        case (GEOMETRY_SPHERE_TYPE_ID): return sphere_normal(p, data);
    }
}

unsigned int get_material_id(__global float* data) {
    // material id is stored in the first index
    return data[0];
}
