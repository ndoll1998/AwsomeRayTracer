#pragma once
#include "include/_defines.h"
#include "src/kernels/structs.cl"

/*** Sphere ***/

void sphere_apply(__global float* data, Sphere* sphere) {
    // read data into sphere
    sphere->center = (float3)(data[1], data[2], data[3]);
    sphere->radius = data[4];
}

int sphere_cast(Ray* ray, Geometry* geometry, float* t) {
    // create sphere from data
    Sphere s; sphere_apply(geometry->data, &s);
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
    return (*t > 0);
}

float3 sphere_normal(float3 p, Geometry* geometry){
    // create sphere from data
    Sphere s; sphere_apply(geometry->data, &s);
    // compute normal at position p on surface
    return normalize(p - s.center);
}


/*** functions ***/

unsigned int geometry_get_type_size(unsigned int geometry_type) {
    // return size of type specified by type-id
    switch(geometry_type) {
        case (GEOMETRY_SPHERE_TYPE_ID): return GEOMETRY_SPHERE_TYPE_SIZE;
        default: break;
    }
}

int geometry_cast_ray(Ray* ray, Geometry* geometry, float* t) {
    // cast to geometry specified by type-id
    switch(geometry->type_id) {
        case (GEOMETRY_SPHERE_TYPE_ID): return sphere_cast(ray, geometry, t);
    }
}

float3 geometry_get_normal(float3 p, Geometry* geometry) {
    // get normal on surface of geometry specified by type and data
    switch(geometry->type_id) {
        case (GEOMETRY_SPHERE_TYPE_ID): return sphere_normal(p, geometry);
    }
}

unsigned int geometry_get_material_id(Geometry* geometry) {
    // material id is stored in the first index
    return geometry->data[0];
}
