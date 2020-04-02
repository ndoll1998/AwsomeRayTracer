#include "include/_defines.h"
#include "src/kernels/ray.cl"
#include "src/kernels/structs.cl"
#include "src/kernels/material.cl"
#include "src/kernels/light.cl"
#include "src/kernels/utils.cl"

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

int camera_cast_ray(
    // ray and return color
    Ray* ray,
    float3* color,
    // containers
    Container* geometries,
    Container* materials,
    Container* lights,
    // ambient
    float3 ambient,
    // globals
    Globals* globals
) {
    //  cast ray to all geometries
    Geometry closest; float t;
    if (ray_cast_to_geometries(ray, geometries, &closest, &t, globals)) {
        // get intersection point
        float3 p = ray_advance(ray, t - EPS);
        // get material
        unsigned int material_id = geometry_get_material_id(&closest);
        Material material; material_get(material_id, materials, &material);
        // get normal of ray on geometry
        float3 normal = geometry_get_normal(p, &closest, globals);
        // get attenuation and light-color
        float3 attenuation = material_get_attenuation(p, ray->direction, normal, &material, globals);
        float3 light_color = light_get_total_light(p, ray->direction, normal, &material, lights, geometries, ambient, globals);
        // combine colors
        *color = clamp(light_color * attenuation, 0.0f, 1.0f);
        // get scatter node and update ray
        return material_get_scatter_ray(p, ray->direction, normal, &material, ray, globals);
    } else {
        // fill with background color
        float t = 0.5 * (1.0 - ray->direction.z);
        *color = (1 - t) + (float3)(0.5, 0.7, 1.0) * t;
        // ray missed
        return 0;
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
    // base color
    float3 color = (float3)(1.0f, 1.0f, 1.0f);
    // scatter ray at most n times
    for (int i = 0; i < MAX_RECURSION_DEPTH; i++) {
        // cast ray
        float3 ray_color; int scatters = camera_cast_ray(ray, &ray_color, geometries, materials, lights, ambient, globals);
        // update color
        color *= ray_color;
        // only continue if scatters
        if (!scatters) { break; }
    }
    return color;    
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
    // initialize seeds for random number generation
    globals.seed0 = y;
    globals.seed1 = x;
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
    unsigned int            n_geometry_bytes,
    // local geometry memory
    __local float*          loc_geometry_data,
    __local unsigned int*   loc_geometry_ids,
    // materials
    __global float*         material_data,
    __global unsigned int*  material_ids,
    unsigned int            n_materials,
    unsigned int            n_material_bytes,
    // local material memory
    __local float*          loc_material_data,
    __local unsigned int*   loc_material_ids,
    // lights
    __global float*         light_data,
    __global unsigned int*  light_ids,
    unsigned int            n_lights,    
    unsigned int            n_light_bytes,
    // local material memory
    __local float*          loc_light_data,
    __local unsigned int*   loc_light_ids,
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

    // read globals to private memory
    Globals globals = all_globals[i];

    // read ids to local memory
    global_to_local((__global char*)geometry_ids, (__local char*)loc_geometry_ids, n_geometries * sizeof(unsigned int));
    global_to_local((__global char*)material_ids, (__local char*)loc_material_ids, n_materials * sizeof(unsigned int));
    global_to_local((__global char*)light_ids,    (__local char*)loc_light_ids,    n_lights * sizeof(unsigned int));
    // read data to local memory
    global_to_local((__global char*)geometry_data, (__local char*)loc_geometry_data, n_geometry_bytes);
    global_to_local((__global char*)material_data, (__local char*)loc_material_data, n_material_bytes);
    global_to_local((__global char*)light_data,    (__local char*)loc_light_data,    n_light_bytes);

    // create containers
    Container geometries = (Container){loc_geometry_data, loc_geometry_ids, n_geometries};
    Container materials  = (Container){loc_material_data, loc_material_ids, n_materials};
    Container lights     = (Container){loc_light_data,    loc_light_ids,    n_lights};

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
        float u = 2 * randf(&globals) - 1;
        float v = 2 * randf(&globals) - 1;
        // create ray throu pixel and get its color
        Ray ray; camera_get_ray_throu_pixel(&ray, x + u, y+ v, w, h, cam, &globals);
        color += camera_get_ray_color(&ray, &geometries, &materials, &lights, ambient, &globals);
    }

    // apply gamma correction
    color = sqrt(color / antialiasing_n_samples);
    // clamp color values between 0 and 255
    color = clamp(color, 0.0f, 1.0f); color *= 255;
    // apply color to pixel
    pixels[i*4+0] = color.x;
    pixels[i*4+1] = color.y;
    pixels[i*4+2] = color.z;
    pixels[i*4+3] = 255;

    // save globals for next iteration
    all_globals[i] = globals;
}