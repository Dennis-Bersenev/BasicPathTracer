#include <crtdbg.h>
#include <stdlib.h>
#include <crtdbg.h>
#include <io.h>
#include <iostream>

#include "camera.h"
#include "sphere.h"
#include "triangle.h"
#include "cube.h"
#include "torus.h"
#include "curve.h"
#include "plane.h"

#ifdef   _DEBUG
#define  SET_CRT_DEBUG_FIELD(a) \
            _CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#define  CLEAR_CRT_DEBUG_FIELD(a) \
            _CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else
#define  SET_CRT_DEBUG_FIELD(a)   ((void) 0)
#define  CLEAR_CRT_DEBUG_FIELD(a) ((void) 0)
#endif

/**
* Recursive function responsible for producing final colour of each sample, at each step attenuating reflected colours.
* @param r - initially the sample ray through the pixel whose final colour is to be computed, subsequent calls being invoked
*            on scattered rays from valid intersections.
* @param world - container for all the objects in the scene.
* @param depth - specifies the ray depth; how many times the ray has bounced about the scene..
* @return - final colour of the sample.
*/
vec3 colour(const ray& r, hitable * world, int depth)
{
    hit_record rec;
    material closest_mat;
    if (world->hit(r, 0.0001, FLT_MAX, rec, closest_mat))
    {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && closest_mat.scatter(r, rec, attenuation, scattered))
        {
            return attenuation * colour(scattered, world, depth + 1);
        }
        else
            return vec3(0, 0, 0);
    }
    //Background colour
    vec3 unit_direction = unit_vector(r.direction());
    double t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1, 1, 1) + t * vec3(0.5, 0.7, 1);
}


void render()
{
    //Standard World setup
    const int num_objects = 1;
    hitable* objects[num_objects];
    //objects[0] = new sphere(vec3(0, -100.5, -1), 100, material(vec3(0.8, 0.8, 0.0), material_type::lambertian));
    //objects[2] = new sphere(vec3(2.0, 0, -1), 0.55, material(vec3(0.0, 0.2, 0.8), material_type::lambertian));
    //objects[0] = new torus(vec3(2.0, 0, -1), unit_vector(vec3(1, 0, 1)), 2, 0.5, material(vec3(0.0, 0.2, 0.8), material_type::lambertian));
    objects[0] = new cube(material(vec3(0.8, 0.3, 0.3), material_type::lambertian));
    
    hitable* world = new hitable_list(objects, num_objects);

    //Standard render setup
    int nx = 200;
    int ny = 100;
    int ns = 20;

    //Cam setup
    vec3 lookfrom(2, 2, 8);
    vec3 lookat(0, 0, -1);

    camera cam(lookfrom, lookat, vec3(0, 1, 0), 45, double(nx) / double(ny));
    

    double u, v;
    ray r;
    vec3 col;
    
    FILE* fd;
    if (fopen_s(&fd, "out/test_cube_new.ppm", "w") != 0 || fd == NULL)
        exit(errno);

    // Use std out to write the file, then restore its reference to the console
    int og_holder = _dup(_fileno(stdout));

    if (_dup2(_fileno(fd), _fileno(stdout)) != 0)
        exit(errno);

    std::cout << "P3\n" << nx << " " << ny << "\n255\n";


    for (int j = ny - 1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            col = vec3(0, 0, 0);
            for (int s = 0; s < ns; s++)
            {
                u = double(i + random_double()) / double(nx);
                v = double(j + random_double()) / double(ny);

                r = cam.get_ray(u, v);
                col += colour(r, world, 0);
            }
            col /= ns;
            //Gamma correction
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
    if (fflush(stdout) != 0)
        exit(errno);
    if (fclose(fd) != 0)
        exit(errno);

    if (_dup2(og_holder, _fileno(stdout)) != 0)
        exit(errno);
    
    delete world;
}

int main() {
    
    // Checking for mem-leaks
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtMemState state;
    _CrtMemCheckpoint(&state);

    render();

    _CrtMemDumpAllObjectsSince(&state);

    // Set the debug-heap flag so that memory leaks are reported when
    // the process terminates. Then, exit.
    SET_CRT_DEBUG_FIELD(_CRTDBG_LEAK_CHECK_DF);
    return 0;
}