#pragma once
#include "src/kernels/random.cl"
#include "src/kernels/structs.cl"

/*** random numbers ***/

#define RANDOM_MAX 4294967296

float randf(Globals* globals) {
    // generate random number in [0, 1)
    return (float)MWC64X_NextUint(&globals->rng) / RANDOM_MAX;
}

/*** Vector-Operations ***/

float3 rotate_along_axis(float3 normalized_v, float3 normalized_axis, float theta) {
    // compute sin and cosin
    float c = cos(theta); 
    float s = sin(theta);
    // compute dot- and cross-product
    float d = dot(normalized_axis, normalized_v);
    float3 w = cross(normalized_axis, normalized_v);
    // Rodrigues' rotation formula
    return normalized_v * c + w * s + normalized_axis * d * (1 - c);
}

float3 reflect(float3 normalized_v, float3 normalized_axis) {
    // reflect v over axis
    return normalized_axis * (dot(normalized_v, normalized_axis) * 2) - normalized_v;
}

int refract(float3 normalized_v, float3 normalized_axis, float r, float3* refracted) {
    // compute discriminant
    float dt = dot(normalized_v, normalized_axis);
    float discr = 1.0 - r * r * (1.0 - dt * dt);
    // check for total internal reflection
    if (discr < 0) return 0;
    // refract ray
    *refracted = ((normalized_v - normalized_axis * dt) * r) - (normalized_axis * sqrt(discr));
    return 1;
}

float3 rand_in_unit_sphere(Globals* globals) {
    // create vector of random numbers
    float3 v = (float3)(
        2 * randf(globals) - 1, 
        2 * randf(globals) - 1, 
        2 * randf(globals) - 1
    );
    // check if v is in unit circle
    if (dot(v, v) < 1) return v;
    // scale v to be in unit circle
    float s = 2 * randf(globals) - 1;
    return normalize(v) * s;
}

/*** Memory ***/

void global_to_local(__global char* global_data, __local char* local_data, unsigned int size) {
    // get local size
    unsigned int d = get_local_size(0);
    unsigned int h = get_local_size(1);
    unsigned int w = get_local_size(2);
    // get local id
    unsigned int z = get_local_id(0);   
    unsigned int y = get_local_id(1);   
    unsigned int x = get_local_id(2);   
    // get flatten id and full size
    unsigned int local_size = w * h * d;
    unsigned int i = x + y * w + z * w * h;

    // load data
    for (int j = 0; j < ceil((float)size / (float)local_size); j++) {
        // current index in data
        unsigned int ii = i + j * local_size;
        // check if index is in bounds
        if (ii >= size) break;
        // read data to local memory
        local_data[ii] = global_data[ii];
    }

    // wait for all workers to finish
    barrier(CLK_LOCAL_MEM_FENCE);
}

