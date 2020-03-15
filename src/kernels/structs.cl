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
    __global float* data;
    __global unsigned int* type_ids;
    // number of elements in container
    unsigned int n;
} Container;


/*** Compressable ***/

typedef struct Compressable {
    __global float* data;
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


/*** Materials ***/

typedef struct ColorMaterial {
    // color
    float3 color;
    // phong values
    float diffuse, specular, shininess;
} ColorMaterial;

/*** Lights ***/

typedef struct PointLight {
    // position and color
    float3 position;
    float3 color;
} PointLight;

