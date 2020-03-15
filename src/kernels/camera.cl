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

float3 camera_get_ray_color(
    // vision ray to get color of
    Ray* ray,
    // containers
    Container* geometries,
    Container* materials,
    Container* lights
) {
    // cast ray to all geometries
    Geometry closest; float t;
    if (ray_cast_to_geometries(ray, geometries, &closest, &t) && (t > 0)) {
        // get intersection point
        float3 p = ray_advance(ray, t - EPS);
        // get normal at intersection point
        float3 normal = geometry_get_normal(p, &closest);
        // get material
        unsigned int material_id = geometry_get_material_id(&closest);
        Material material; material_get(material_id, materials, &material);
        // get base and light color at position
        float3 base_color = material_get_base_color(p, &material);
        float3 light_color = light_get_total_light(p, ray->direction, normal, &material, lights, geometries);
        // return combined colors
        return base_color * light_color;
    } else {
        return (0, 0, 0);
    }
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

    // create ray throu pixel
    Ray r; camera_get_ray_throu_pixel(&r, x, y, w, h, cam);
    // get color of ray
    float3 color = camera_get_ray_color(&r, &geometries, &materials, &lights);
    // clamp color values between 0 and 255
    color = clamp(color, 0, 1); color *= 255;
    // apply color to pixel
    pixels[i+0] = color.x;
    pixels[i+1] = color.y;
    pixels[i+2] = color.z;
    pixels[i+3] = 255;
}