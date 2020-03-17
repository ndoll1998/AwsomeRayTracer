#pragma once

/*** Basics ***/

#define EPS 1e-5
#define MAX_RECURSION_DEPTH 50

/*** Geometries ***/

/* Sphere */
#define GEOMETRY_SPHERE_TYPE_ID 0
#define GEOMETRY_SPHERE_TYPE_SIZE 1 + 4


/*** Materials ***/

/* Diffuse Material */
#define MATERIAL_DIFFUSEMATERIAL_TYPE_ID 0
#define MATERIAL_DIFFUSEMATERIAL_TYPE_SIZE 6
/* Metal Material */
#define MATERIAL_METALMATERIAL_TYPE_ID 1
#define MATERIAL_METALMATERIAL_TYPE_SIZE 7


/*** Lights ***/

/* Point Light */
#define LIGHT_POINTLIGHT_TYPE_ID 0
#define LIGHT_POINTLIGHT_TYPE_SIZE 6
