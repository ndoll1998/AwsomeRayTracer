#pragma once

/*** Basics ***/

#define EPS 1e-5f   // make sure this is of float type
#define MAX_RECURSION_DEPTH 5

/*** Geometries ***/

/* Sphere */
#define GEOMETRY_SPHERE_TYPE_ID 0
#define GEOMETRY_SPHERE_TYPE_SIZE 1 + 4


/*** Materials ***/

/* Diffuse Material */
#define MATERIAL_DIFFUSE_TYPE_ID 0
#define MATERIAL_DIFFUSE_TYPE_SIZE 6
/* Metal Material */
#define MATERIAL_METAL_TYPE_ID 1
#define MATERIAL_METAL_TYPE_SIZE 7
/* Dielectric Material */
#define MATERIAL_DIELECTRIC_TYPE_ID 2
#define MATERIAL_DIELECTRIC_TYPE_SIZE 4


/*** Lights ***/

/* Point Light */
#define LIGHT_POINTLIGHT_TYPE_ID 0
#define LIGHT_POINTLIGHT_TYPE_SIZE 6
