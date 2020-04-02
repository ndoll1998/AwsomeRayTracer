#pragma once
#include "include/_defines.h"
#include "src/kernels/ray.cl"
#include "src/kernels/structs.cl"

/*** Sphere ***/

void sphere_apply(__local float* data, Sphere* sphere) {
    // read data into sphere
    sphere->center = (float3)(data[1], data[2], data[3]);
    sphere->radius = data[4];
}

int sphere_cast(Ray* ray, Geometry* geometry, float* t, Globals* globals) {
    // create sphere from data
    Sphere s; sphere_apply(geometry->data, &s);
    // analytic solution of sphere-ray-intersection
    float3 L = ray->origin - s.center;
    float b = dot(ray->direction, L) * 2;
    float c = dot(L, L) - s.radius * s.radius;
    // solve quadratic function
    float discr = b * b - 4 * c;
    // no intersection
    if (discr < 0) return 0;
    // one intersection
    else if (discr == 0) { *t = -0.5 * b; }
    // two intersections
    else {
        float sqrt_discr = sqrt(discr);
        float q = (b > 0)? 
            -0.5 * (b + sqrt_discr):
            -0.5 * (b - sqrt_discr);
        *t = min(q, c / q);
    }
    return (*t > 0);
}

float3 sphere_normal(float3 p, Geometry* geometry, Globals* globals){
    // create sphere from data
    Sphere s; sphere_apply(geometry->data, &s);
    // compute normalized normal at position p on surface
    return (p - s.center) / s.radius;
}

/*** Plane ***/

float3 _plane_normal(float3 p, Plane* plane) {
    // return normal facing towards p
    float3 u = plane->origin - p;
    // compute normal facing towards given point
    return (dot(u, plane->normal) < 0)? plane->normal : -plane->normal;
}

int _plane_cast(Ray* ray, Plane* plane, float* t) {
    // get normal facing towards ray origin
    float3 normal = _plane_normal(ray->origin, plane);
    // get normal and check if plane and ray are aligned
    float denom = dot(plane->normal, ray->direction); 
    if (denom < -EPS) { 
        // compute distance to intersection point
        float3 p = plane->origin - ray->origin; 
        *t = dot(p, plane->normal) / denom;
        return (*t >= EPS);
    } 
    // no intersection
    return 0; 
}

void plane_apply(__local float* data, Plane* plane) {
    // read origin and normal
    plane->origin = (float3)(data[1], data[2], data[3]);
    plane->normal = (float3)(data[4], data[5], data[6]);
}

int plane_cast(Ray* ray, Geometry* geometry, float* t, Globals* globals) {
    // create plane from data
    Plane plane; plane_apply(geometry->data, &plane);
    // check intersection with plane
    return _plane_cast(ray, &plane, t);
}

float3 plane_normal(float3 p, Geometry* geometry, Globals* globals) {
    // create plane from data
    Plane plane; plane_apply(geometry->data, &plane);
    // return normal facing towards point p
    return _plane_normal(p, &plane);
}

/*** Triangle ***/

float3 _triangle_normal(float3 p, Triangle* T) {
    // get direction
    float3 u = T->A - p;
    // compute normal facing towards given point
    float3 n = cross(T->A - T->B, T->A - T->C);
    return (dot(u, n) < 0)? n : -n;
}

void triangle_apply(__local float* data, Triangle* triangle) {
    // read points from data
    triangle->A = (float3)(data[1], data[2], data[3]);
    triangle->B = (float3)(data[4], data[5], data[6]);
    triangle->C = (float3)(data[7], data[8], data[9]);
}

int triangle_cast(Ray* ray, Geometry* geometry, float* t, Globals* globals) {
    // create Triangle from data
    Triangle T; triangle_apply(geometry->data, &T);
    // get unnormalized normal of triangle
    float3 normal = _triangle_normal(ray->origin, &T);
    // create plane containing triangle and check intersection
    Plane plane = (Plane){T.A, normal};
    if (!_plane_cast(ray, &plane, t)) return 0;
    // get point of intersection
    float3 P = ray_advance(ray, *t);
    // check with all edges of the triangle
    // AB
    float3 AB_AP = cross(T.B - T.A, P - T.A);
    if (dot(AB_AP, normal) > 0) return 0;
    // AB
    float3 BC_BP = cross(T.C - T.B, P - T.B);
    if (dot(BC_BP, normal) > 0) return 0;
    // AB
    float3 CA_CP = cross(T.A - T.C, P - T.C);
    if (dot(CA_CP, normal) > 0) return 0;
    // P is inside triangle
    return 1;
}

float3 triangle_normal(float3 p, Geometry* geometry, Globals* globals) {
    // create triangle from data
    Triangle T; triangle_apply(geometry->data, &T);
    // return normalized normal facing towards p
    return normalize(_triangle_normal(p, &T));
}

/*** functions ***/

unsigned int geometry_get_type_size(unsigned int geometry_type) {
    // return size of type specified by type-id
    switch(geometry_type) {
        case (GEOMETRY_SPHERE_TYPE_ID):     return GEOMETRY_SPHERE_TYPE_SIZE;
        case (GEOMETRY_PLANE_TYPE_ID):      return GEOMETRY_PLANE_TYPE_SIZE;
        case (GEOMETRY_TRIANGLE_TYPE_ID):   return GEOMETRY_TRIANGLE_TYPE_SIZE;
    }
}

int geometry_cast_ray(Ray* ray, Geometry* geometry, float* t, Globals* globals) {
    // cast to geometry specified by type-id
    switch(geometry->type_id) {
        case (GEOMETRY_SPHERE_TYPE_ID):     return sphere_cast(ray, geometry, t, globals);
        case (GEOMETRY_PLANE_TYPE_ID):      return plane_cast(ray, geometry, t, globals);
        case (GEOMETRY_TRIANGLE_TYPE_ID):   return triangle_cast(ray, geometry, t, globals);
    }
}

float3 geometry_get_normal(float3 p, Geometry* geometry, Globals* globals) {
    // get normal on surface of geometry specified by type and data
    switch(geometry->type_id) {
        case (GEOMETRY_SPHERE_TYPE_ID):     return sphere_normal(p, geometry, globals);
        case (GEOMETRY_PLANE_TYPE_ID):      return plane_normal(p, geometry, globals);
        case (GEOMETRY_TRIANGLE_TYPE_ID):   return triangle_normal(p, geometry, globals);
    }
}

unsigned int geometry_get_material_id(Geometry* geometry) {
    // material id is stored in the first index
    return geometry->data[0];
}
