#include "include/_defines.h"
#include "src/kernels/structs.cl"
#include "src/kernels/ray.cl"
#include "src/kernels/material.cl"
#include "src/kernels/light.cl"
#include "src/kernels/utils.cl"

void camera_get_ray_throu_pixel(Ray *ray, 
    unsigned int x, unsigned int y, 
    unsigned int w, unsigned int h,
    Camera cam
) {
    // compute vertical field of view
    float v_fov = ((float)h / (float)w) * cam.fov;
    // compute angles betweem ray and camera-direction
    float xrot = cam.fov * ((float)x / (float)w - 0.5);
    float yrot = v_fov * ((float)y / (float)h - 0.5);
    // rotate cam-direction by angles
    float3 dir = rotate_along_axis(cam.dir, cam.up, xrot);
    dir = rotate_along_axis(dir, cross(cam.dir, cam.up), yrot);
    // save origin and direction in ray
    ray->origin = cam.pos;
    ray->direction = dir;
}

void camera_cast_ray(
    // current and child node
    RecursionNode* cur_node,
    // containers
    Container* geometries,
    Container* materials,
    Container* lights
) {
    // get ray from current node
    Ray ray = cur_node->ray;
    //  cast ray to all geometries
    Geometry closest; float t;
    if (ray_cast_to_geometries(&ray, geometries, &closest, &t)) {
        // get intersection point
        float3 p = ray_advance(&ray, t - EPS);
        // get material
        unsigned int material_id = geometry_get_material_id(&closest);
        Material material; material_get(material_id, materials, &material);
        // get normal of ray on geometry
        float3 normal = geometry_get_normal(p, &closest);
        // fill current node
        cur_node->color_a = material_get_base_color(p, &material);
        cur_node->color_b = light_get_total_light(p, ray.direction, normal, &material, lights, geometries);
        // fill reflect node
        cur_node->child_reflect_node->ray.origin = p;
        cur_node->child_reflect_node->ray.direction = -reflect(ray.direction, normal);
        cur_node->child_reflect_node->scale = material_get_reflection(p, &material);
    } else {
        // fill current node
        cur_node->color_a = (float3)(0, 0, 0);
        cur_node->color_b = (float3)(0, 0, 0);
        // fill child nodes - break recursion on scale 0
        cur_node->child_reflect_node->scale = 0;
    }
}

float3 camera_get_ray_color(
    // initial ray
    Ray* ray,
    // containers
    Container* geometries,
    Container* materials,
    Container* lights
) {
    // allocate array to store recursive tree
    RecursionNode recursion_nodes[MAX_RECURSION_DEPTH + 1];
    unsigned int n_inner_nodes = MAX_RECURSION_DEPTH;
    unsigned int n_leaf_nodes = 1;
    // build root node
    recursion_nodes[0].ray.origin = ray->origin;
    recursion_nodes[0].ray.direction = ray->direction;
    recursion_nodes[0].scale = 1;

    // build recusion tree
    int j; RecursionNode* node = recursion_nodes;
    // loop over all inner nodes of the recursion tree
    for (j = 0; j < n_inner_nodes; j++) {
        // set child nodes
        node->child_reflect_node = recursion_nodes + tree_get_child(j, 0, 1);
        // build recursive node and go to next node
        camera_cast_ray(node++, geometries, materials, lights);
    }
    RecursionNode trash_node;
    // loop over all leaf nodes
    for (int k = 0; k < n_leaf_nodes; k++) {
        // set child node to trash node
        node->child_reflect_node = &trash_node;
        // fill current node
        camera_cast_ray(node, geometries, materials, lights);
        // compute recursion result of leaf node
        (node++)->result_color = node->color_a * node->color_b;
    }

    // get last inner node
    node = recursion_nodes + n_inner_nodes - 1;
    // solve recursion
    for (int k = 0; k < n_inner_nodes; k++) {
        // compute recusion result of current node
        (node--)->result_color = node->scale * (node->color_a + node->child_reflect_node->result_color) * node->color_b;
    }

    // return resulting color of root node
    return recursion_nodes->result_color;
}


/*** kernel ***/

__kernel void camera_get_pixel_color(
    // pixel array (rgba-format)
    __global unsigned char* pixels,
    // geometries
    __global float*         geometry_data,
    __global unsigned int*  geometry_ids,
    unsigned int            n_geometries,
    // materials
    __global float*         material_data,
    __global unsigned int*  material_ids,
    unsigned int            n_materials,
    // lights
    __global float*         light_data,
    __global unsigned int*  light_ids,
    unsigned int            n_lights,
    // camera orientation
    float cam_x, float cam_y, float cam_z,
    float cam_u, float cam_v, float cam_w,
    float cam_a, float cam_b, float cam_c,
    float cam_fov
) {
    // get indices
    unsigned int y = get_global_id(0);
    unsigned int x = get_global_id(1);
    // get image size
    unsigned int h = get_global_size(0);
    unsigned int w = get_global_size(1);
    // compute flatten index
    unsigned int i = (x + y * w) * 4;

    // create camera
    Camera cam = (Camera) {
        (float3)(cam_x, cam_y, cam_z), 
        (float3)(cam_u, cam_v, cam_w), 
        (float3)(cam_a, cam_b, cam_c),
        cam_fov
    };

    // create containers
    Container geometries = (Container){geometry_data, geometry_ids, n_geometries};
    Container materials  = (Container){material_data, material_ids, n_materials};
    Container lights =     (Container){light_data, light_ids, n_lights};

    // create ray throu pixel and get its color
    Ray ray; camera_get_ray_throu_pixel(&ray, x, y, w, h, cam);
    float3 color = camera_get_ray_color(&ray, &geometries, &materials, &lights);

    // clamp color values between 0 and 255
    color = clamp(color, 0, 1); color *= 255;
    // apply color to pixel
    pixels[i+0] = color.x;
    pixels[i+1] = color.y;
    pixels[i+2] = color.z;
    pixels[i+3] = 255;
}