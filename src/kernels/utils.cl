#pragma once

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


/*** Tree-Opertations */

int tree_get_parent(int node, int k) {
    // get parent id from node id
    return (int)((node - 1) / 2);
}

int tree_get_child(int parent, int child_id, int k) {
    // get global child id in tree from local child id
    return k * parent + child_id + 1;
}

int tree_get_size(int h, int k) {
    // handle special case of k = 1
    if (k == 1) { return h+1; }
    // get numer of nodes in at k-tree of height h
    return (k * (h + 1) - 1) / (k - 1);
}
