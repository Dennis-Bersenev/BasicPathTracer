#include <io.h>
#include <iostream>
#include "vec3.h"
#include "random.h"
#include "ray.h"
#include "hitable_list.h"
#include "sphere.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include "triangle.h"
#include "cube.h"




/**
* Recursive function responsible for producing final colour of each sample, at each step attenuating reflected colours.
* @param r - initially the sample ray through the pixel whose final colour is to be computed, subsequent calls being invoked
*            on scattered rays from valid intersections.
* @param world - container for all the objects in the scene.
* @param depth - specifies the ray depth; how many times the ray has bounced about the scene..
* @return - final colour of the sample.
*/
vec3 colour(const ray& r, hitable * world, int depth, const vec3& l)
{
    hit_record rec;
    if (world->hit(r, 0.0001, FLT_MAX, rec))
    {
        ray scattered;
        vec3 attenuation;
        vec3 light = unit_vector(l - rec.p);
        if (depth < 50 && rec.mat->scatter(r, rec, attenuation, scattered, world, light))
        {
            return attenuation * colour(scattered, world, depth + 1, l);
        }
        else
            return vec3(0, 0, 0);
    }
    //Light colour
    
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1, 1, 1) + t * vec3(0.5, 0.7, 1);
    
    //return vec3(1.0, 1.0, 1.0);
}

//Creates a random scene.
hitable* random_scene() {
    int n = 500;
    hitable** list = new hitable * [n + 1];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = random_double();
            vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {  // diffuse
                    list[i++] = new sphere(
                        center, 0.2,
                        new lambertian(vec3(random_double() * random_double(),
                            random_double() * random_double(),
                            random_double() * random_double()))
                    );
                }
                else if (choose_mat < 0.95) { // metal
                    list[i++] = new sphere(
                        center, 0.2,
                        new metal(vec3(0.5 * (1 + random_double()),
                            0.5 * (1 + random_double()),
                            0.5 * (1 + random_double())),
                            0.5 * random_double())
                    );
                }
                else {  // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.5));

    return new hitable_list(list, i);
}


int main() {
    int nx = 800;
    int ny = 600;
    int ns = 20;
    FILE* fd;
    if (fopen_s(&fd, "out/test_refactor.ppm", "w") != 0 || fd == NULL)
        exit(errno);

    if (_dup2(_fileno(fd), _fileno(stdout)) != 0)
        exit(errno);

    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    
    /*Incorporating light
    //Point
    vec3 light_pos = vec3(2, 20, 5);

    //Directional
    vec3 light_dir = vec3(0.5, -1, -2);

    vec3 l = light_pos;
    */


    /*Building a triangle
    vec3 vertices[3];
    
    vec3 a = vec3(0.5, 0.0, -1);
    vec3 b = vec3(0.75, 0.70, -1);
    vec3 c = vec3(1.0, 0.0, -1);
    */
    hitable* objects[3];
    objects[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
    objects[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    objects[2] = new sphere(vec3(2.0, 0, -1), 0.55, new lambertian(vec3(0.0, 0.2, 0.8)));
    //objects[2] = new triangle(a, b, c, new lambertian(vec3(0.0, 0.2, 0.8)));
    hitable* world = new hitable_list(objects, 3);
    

    /*Building a cube, setup:
    
    hitable* list[1];
    cube * c1 = new cube(new lambertian(vec3(0.8, 0.3, 0.3)));
    c1->rotate_cube_x(30.0f);
    c1->rotate_cube_z(70.0f);
    list[0] = c1;
    hitable_list* world = new hitable_list(list, 1);
    */
    //Cam setup
    vec3 lookfrom(2, 3, 8);
    vec3 lookat(0, 0, 0);
    float dist_to_focus = 10.0;
    float aperture = 0.1;

    camera cam(lookfrom, lookat, vec3(0, 1, 0), 30, float(nx) / float(ny), aperture, dist_to_focus);

    float u, v;
    ray r;
    vec3 col;



    for (int j = ny - 1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            col = vec3(0, 0, 0);
            for (int s = 0; s < ns; s++)
            {
                u = float(i + random_double()) / float(nx);
                v = float(j + random_double()) / float(ny);

                r = cam.get_ray(u, v);
                col += colour(r, world, 0, l);
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
    fclose(fd);
}