#pragma once
#include "src/kernels/random.cl"

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

typedef struct DiffuseMaterial {
    // color
    float3 color;
    // phong values
    float diffuse, specular, shininess;
    // attenuation
    float attenuation;
} DiffuseMaterial;

/*** Lights ***/

typedef struct PointLight {
    // position and color
    float3 position;
    float3 color;
} PointLight;


/*** Recursion Tree Node ***/

typedef struct RecursionNode {
    // ray of node
    Ray ray;
    int hit;
    // recursive parameters - used in build
    float3 color_a, color_b;
    float scale;
    // recusive result - used in solve
    float3 result_color;
    // child nodes
    struct RecursionNode* parent_node;
    struct RecursionNode* child_scatter_node;
} RecursionNode;


/*** Globals ***/

typedef struct Globals {
    // values that need to be globally accessable in each work-item but can differ between work-items
    // random number generator
    mwc64x_state_t rng;
} Globals;
