#pragma once

float3 rotate_along_axis(float3 vec, float3 axis, float theta) {
    // compute sin and cosin
    float c = cos(theta); 
    float s = sin(theta);
    // compute dot- and cross-product
    float d = dot(axis, vec);
    float3 w = cross(axis, vec);
    // Rodrigues' rotation formula
    return vec * c + w * s + axis * d * (1 - c);
}
