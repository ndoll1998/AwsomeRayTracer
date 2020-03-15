#include "src/kernels/structs.cl"
#include "src/kernels/ray.cl"
#include "src/kernels/material.cl"
#include "src/kernels/utils.cl"

void get_ray_thou_pixel(Ray *ray, 
    unsigned int x, unsigned int y, 
    unsigned int w, unsigned int h,
    float3 cam_pos, float3 cam_dir, float3 cam_up, float cam_fov
) {
    // compute vertical field of view
    float v_fov = ((float)h / (float)w) * cam_fov;
    // compute angles betweem ray and camera-direction
    float xrot = cam_fov * ((float)x / (float)w - 0.5);
    float yrot = v_fov * ((float)y / (float)h - 0.5);
    // rotate cam-direction by angles
    float3 dir = rotate_along_axis(cam_dir, cam_up, xrot);
    dir = rotate_along_axis(dir, cross(cam_dir, cam_up), yrot);
    // save origin and direction in ray
    ray->origin = cam_pos;
    ray->direction = dir;
}


__kernel void get_pixel_color(
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

    // camera orientation
    float3 cam_pos = (float3)(cam_x, cam_y, cam_z);
    float3 cam_dir = (float3)(cam_u, cam_v, cam_w);
    float3 cam_up  = (float3)(cam_a, cam_b, cam_c);

    float3 color;
    // create ray throu pixel
    Ray r; get_ray_thou_pixel(&r, x, y, w, h, cam_pos, cam_dir, cam_up, cam_fov);
    // cast ray to all geometries
    __global float* closest_data; unsigned int closest_geometry; float t;
    int hit = cast_ray(&r, geometry_data, geometry_ids, n_geometries, &closest_data, &closest_geometry, &t);
    // no intersection
    if (!hit) { color = (0, 0, 0); } 
    // intersection
    else { 
        // get intersection point
        float3 ipoint = advance_ray(&r, t);
        // get normal at intersection point
        float3 normal = get_normal(ipoint, geometry_ids[closest_geometry], closest_data);
        // get base color from material
        unsigned int material_id = get_material_id(closest_data);
        color = get_material_color(ipoint, material_id, material_data, material_ids, n_materials);
        color *= -dot(r.direction, normal); 
    }

    color *= 255;
    // apply color
    pixels[i+0] = color.x;
    pixels[i+1] = color.y;
    pixels[i+2] = color.z;
    pixels[i+3] = 255;

}