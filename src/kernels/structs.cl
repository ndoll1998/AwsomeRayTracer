#pragma once

/*** Camera ***/

typedef struct Camera {
    // position and orientation
    float3 pos, dir, up;
    // field of view
    float fov;
} Camera;

/*** Ray ***/

typedef struct Ray{
    // origin and direction
    float3 origin;
    float3 direction;
} Ray;


/*** Container ***/

typedef struct Container {
    // data and type-ids
    __local float* data;
    __local unsigned int* type_ids;
    // number of elements in container
    unsigned int n;
} Container;


/*** Compressable ***/

typedef struct Compressable {
    __local float* data;
    unsigned int type_id;
} Compressable;

#define Material Compressable
#define Geometry Compressable
#define Light Compressable


/*** Geometries ***/

typedef struct Sphere {
    // center and radius
    float3 center;
    float radius;
} Sphere;

typedef struct Plane {
    // origin and normal
    float3 origin;
    float3 normal;
} Plane;


/*** Materials ***/

typedef struct DiffuseMaterial {
    // attenuation
    float3 attenuation;
    // phong values
    float diffuse, specular, shininess;
} DiffuseMaterial;

typedef struct MetalMaterial {
    // base on diffuse material
    DiffuseMaterial base;
    // fuzzyness
    float fuzz;
} MetalMaterial;

typedef struct DielectricMaterial {
    // phong values
    float diffuse, specular, shininess;
    // index of refraction
    float ior;
} DielectricMaterial;

/*** Lights ***/

typedef struct PointLight {
    // position and color
    float3 position;
    float3 color;
} PointLight;

/*** Globals ***/
// if changing this struct remember to also adjust the allocated size in host code

typedef struct Globals {
    // values that need to be globally accessable in each work-item but can differ between work-items
    // random number generator
    unsigned int seed0, seed1;
} Globals;
