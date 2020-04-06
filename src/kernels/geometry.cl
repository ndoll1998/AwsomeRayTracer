#pragma once
#include "include/_defines.h"
#include "src/kernels/ray.cl"
#include "src/kernels/structs.cl"
#include "src/kernels/utils.cl"

/*** Sphere ***/

float3 sphere_get_center(Geometry* geometry) {
    // return center of sphere
    return (float3)(geometry->data[1], geometry->data[2], geometry->data[3]);
}

int sphere_cast(Ray* ray, Geometry* geometry, float* t, Globals* globals) {
    // get sphere information
    float3 center = sphere_get_center(geometry);
    float radius = geometry->data[4];
    // analytic solution of sphere-ray-intersection
    float3 L = ray->origin - sphere_get_center(geometry);
    float b = dot(ray->direction, L) * 2;
    float c = dot(L, L) - (radius * radius);
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
    // get sphere information
    float3 center = sphere_get_center(geometry);
    float radius = geometry->data[4];
    // compute normalized normal at position p on surface
    return (p - center) / radius;
}


/*** Plane ***/

int _plane_cast(Ray* ray, float3 o, float3 n, float* t) {
    // get normal facing towards ray origin
    n = face_normal_towards_point(ray->origin, o, n);
    // get normal and check if plane and ray are aligned
    float denom = dot(n, ray->direction); 
    if (denom < -EPS) { 
        // compute distance to intersection point
        float3 p = o - ray->origin; 
        *t = dot(p, n) / denom;
        return (*t >= EPS);
    } 
    // no intersection
    return 0; 
}

float3 plane_get_origin(Geometry* geometry) {
    // return origin of plane
    return (float3)(geometry->data[1], geometry->data[2], geometry->data[3]);
}

float3 plane_get_normal(Geometry* geometry) {
    // return normal of plane
    return (float3)(geometry->data[4], geometry->data[5], geometry->data[6]);
}

int plane_cast(Ray* ray, Geometry* geometry, float* t, Globals* globals) {
    // check intersection with plane
    return _plane_cast(ray, plane_get_origin(geometry), plane_get_normal(geometry), t);
}

float3 plane_normal(float3 p, Geometry* geometry, Globals* globals) {
    // return normal facing towards point p
    return face_normal_towards_point(p, plane_get_origin(geometry), plane_get_normal(geometry));
}


/*** Triangle ***/

float3 _triangle_normal(float3 p, float3 A, float3 B, float3 C) {
    // get direction
    float3 u = A - p;
    // compute normal facing towards given point
    float3 n = cross(A - B, A - C);
    return face_normal_towards_point(p, A, n);
}

float3 triangle_get_A(Geometry* geometry) {
    // return A
    return (float3)(geometry->data[1], geometry->data[2], geometry->data[3]);
}

float3 triangle_get_B(Geometry* geometry) {
    // return A
    return (float3)(geometry->data[4], geometry->data[5], geometry->data[6]);
}

float3 triangle_get_C(Geometry* geometry) {
    // return A
    return (float3)(geometry->data[7], geometry->data[8], geometry->data[9]);
}

int triangle_cast(Ray* ray, Geometry* geometry, float* t, Globals* globals) {
    // create Triangle from data
    float3 A = triangle_get_A(geometry);
    float3 B = triangle_get_B(geometry);
    float3 C = triangle_get_C(geometry);
    // get unnormalized normal of triangle
    float3 normal = _triangle_normal(ray->origin, A, B, C);
    // cast ray to plane
    if (!_plane_cast(ray, A, normal, t)) return 0;
    // get point of intersection
    float3 P = ray_advance(ray, *t);
    // check with all edges of the triangle
    // AB
    float3 AB_AP = cross(B - A, P - A);
    if (dot(AB_AP, normal) > 0) return 0;
    // AB
    float3 BC_BP = cross(C - B, P - B);
    if (dot(BC_BP, normal) > 0) return 0;
    // AB
    float3 CA_CP = cross(A - C, P - C);
    if (dot(CA_CP, normal) > 0) return 0;
    // P is inside triangle
    return 1;
}

float3 triangle_normal(float3 p, Geometry* geometry, Globals* globals) {
    // create Triangle from data
    float3 A = triangle_get_A(geometry);
    float3 B = triangle_get_B(geometry);
    float3 C = triangle_get_C(geometry);
    // return normalized normal facing towards p
    return normalize(_triangle_normal(p, A, B, C));
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
