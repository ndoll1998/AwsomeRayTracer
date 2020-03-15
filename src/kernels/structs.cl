#pragma once

/*** Ray ***/

typedef struct Ray{
    // origin and direction
    float3 origin;
    float3 direction;
} Ray;


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
} ColorMaterial;