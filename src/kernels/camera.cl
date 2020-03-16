#include "include/_defines.h"
#include "src/kernels/structs.cl"
#include "src/kernels/ray.cl"
#include "src/kernels/material.cl"
#include "src/kernels/light.cl"
#include "src/kernels/utils.cl"
#include "src/kernels/random.cl"

void camera_get_ray_throu_pixel(Ray *ray, 
    float x, float y, 
    unsigned int w, unsigned int h,
    Camera cam,
    // globals
    Globals* globals

) {
    // compute vertical field of view
    float v_fov = ((float)h / (float)w) * cam.fov;
    // compute angles betweem ray and camera-direction
    float x_off = cam.fov * (x / (float)w - 0.5);
    float y_off = v_fov * (y / (float)h - 0.5);
    // rotate cam-direction by angles
    float3 dir = cam.dir + cross(cam.dir, cam.up) * x_off + cam.up * y_off;
    // save origin and direction in ray
    ray->origin = cam.pos;
    ray->direction = normalize(dir);
}

void camera_cast_ray(
    // current node
    RecursionNode* cur_node,
    // containers
    Container* geometries,
    Container* materials,
    Container* lights,
    // ambient
    float3 ambient,
    // globals
    Globals* globals

) {
    // get ray from current node
    Ray ray = cur_node->ray;
    //  cast ray to all geometries
    Geometry closest; float t;
    if (ray_cast_to_geometries(&ray, geometries, &closest, &t, globals)) {
        // get intersection point
        float3 p = ray_advance(&ray, t - EPS);
        // get material
        unsigned int material_id = geometry_get_material_id(&closest);
        Material material; material_get(material_id, materials, &material);
        // get normal of ray on geometry
        float3 normal = geometry_get_normal(p, &closest, globals);
        // fill current node
        cur_node->hit = 1;
        cur_node->color_a = material_get_base_color(p, &material, globals);
        cur_node->color_b = light_get_total_light(p, ray.direction, normal, &material, lights, geometries, ambient, globals);
        // fill scatter node
        if (material_get_scatter_ray(p, ray.direction, normal, &material, &cur_node->child_scatter_node->ray, globals))
            cur_node->scale = material_get_attenuation(p, ray.direction, normal, &material, globals);
    } else {
        // ray missed
        cur_node->hit = 0;
    }
}

float3 camera_get_ray_color(
    // initial ray
    Ray* ray,
    // containers
    Container* geometries,
    Container* materials,
    Container* lights,
    // ambient color
    float3 ambient,
    // globals
    Globals* globals
) {
    // allocate array to store recursive tree
    RecursionNode recursion_nodes[MAX_RECURSION_DEPTH + 1];
    unsigned int n_inner_nodes = MAX_RECURSION_DEPTH;
    unsigned int n_leaf_nodes = 1;
    // build root node
    recursion_nodes[0].ray.origin = ray->origin;
    recursion_nodes[0].ray.direction = ray->direction;
    recursion_nodes[0].scale = 1;
    // set parent of root
    RecursionNode root_parent; root_parent.hit = 1;
    recursion_nodes[0].parent_node = & root_parent;

    // build recusion tree
    int j; RecursionNode* node = recursion_nodes;
    // loop over all inner nodes of the recursion tree
    for (j = 0; j < n_inner_nodes; j++) {
        // make sure that parent node hit any geometry
        if (!node->parent_node->hit) continue;
        // set child-parent relation
        node->child_scatter_node = recursion_nodes + tree_get_child(j, 0, 1);
        node->child_scatter_node->parent_node = node;
        // build recursive node and go to next node
        camera_cast_ray(node++, geometries, materials, lights, ambient, globals);
    }
    RecursionNode trash_node;
    // loop over all leaf nodes
    for (int k = 0; k < n_leaf_nodes; k++) {
        // make sure that parent node hit any geometry
        if (!node->parent_node->hit) continue;
        // set child node to trash node
        node->child_scatter_node = &trash_node;
        // fill current node
        camera_cast_ray(node, geometries, materials, lights, ambient, globals);
        // compute recursion result of leaf node
        (node++)->result_color = clamp((1 - node->scale) * node->color_a, 0, 1) * node->color_b;
    }

    // get last inner node
    node = recursion_nodes + n_inner_nodes - 1;
    // solve recursion
    for (int k = 0; k < n_inner_nodes; k++) {
        // check if current node hit any geometry
        if (node->hit) {
            // compute recusion result of current node
            float3 fcolor = (node->color_a * node->scale + node->child_scatter_node->result_color * (1 - node->scale)) * node->color_b;
            node->result_color = clamp(fcolor, 0, 1);
        } else {
            // fill with background color
            float t = 0.5 * (1.0 - node->ray.direction.z);
            node->result_color = (1 - t) + (float3)(0.5, 0.7, 1.0) * t;
        }
        node--;
    }

    // return resulting color of root node
    return recursion_nodes->result_color;
}


/*** kernels ***/

__kernel void camera_prepare_globals(
    __global Globals* all_globals
) {
    // get indices
    unsigned int y = get_global_id(0);
    unsigned int x = get_global_id(1);
    // get image size
    unsigned int h = get_global_size(0);
    unsigned int w = get_global_size(1);
    // compute flatten index
    unsigned int i = x + y * w;

    // read globals to private address space
    Globals globals = all_globals[i];
    // initialize random number generator
    MWC64X_SeedStreams(&globals.rng, y*w, 2 * i);
    // store globals back in global address space
    all_globals[i] = globals;
}

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
    // field of view and antialiasing
    float cam_fov,
    unsigned int antialiasing_n_samples,
    // ambient color
    float ambient_r, float ambient_g, float ambient_b,
    // globals
    __global Globals* all_globals
) {
    // get indices
    unsigned int y = get_global_id(0);
    unsigned int x = get_global_id(1);
    // get image size
    unsigned int h = get_global_size(0);
    unsigned int w = get_global_size(1);
    // compute flatten index
    unsigned int i = x + y * w;

    // build globals
    Globals globals = all_globals[i];

    // create containers
    Container geometries = (Container){geometry_data, geometry_ids, n_geometries};
    Container materials  = (Container){material_data, material_ids, n_materials};
    Container lights =     (Container){light_data, light_ids, n_lights};

    // create camera
    Camera cam = (Camera) {
        (float3)(cam_x, cam_y, cam_z), 
        (float3)(cam_u, cam_v, cam_w), 
        (float3)(cam_a, cam_b, cam_c),
        cam_fov
    };

    // create ambient light color
    float3 ambient = (float3) (ambient_r, ambient_g, ambient_b);

    // initialize color with ray throu middle of pixel
    Ray ray; camera_get_ray_throu_pixel(&ray, x, y, w, h, cam, &globals);
    float3 color = camera_get_ray_color(&ray, &geometries, &materials, &lights, ambient, &globals);
    // antialiasing
    for (int j = 0; j < antialiasing_n_samples - 1; j++) {
        // get random offset from pixel center
        float u = 2 * ((float)MWC64X_NextUint(&globals.rng) / RANDOM_MAX) - 1;
        float v = 2 * ((float)MWC64X_NextUint(&globals.rng) / RANDOM_MAX) - 1;
        // create ray throu pixel and get its color
        Ray ray; camera_get_ray_throu_pixel(&ray, x + u, y+ v, w, h, cam, &globals);
        color += camera_get_ray_color(&ray, &geometries, &materials, &lights, ambient, &globals);
    }

    // apply gamma correction
    color = sqrt(color / antialiasing_n_samples);
    // clamp color values between 0 and 255
    color = clamp(color, 0, 1); color *= 255;
    // apply color to pixel
    pixels[i*4+0] = color.x;
    pixels[i*4+1] = color.y;
    pixels[i*4+2] = color.z;
    pixels[i*4+3] = 255;

    // save globals for next iteration
    all_globals[i] = globals;
}