#pragma once

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
