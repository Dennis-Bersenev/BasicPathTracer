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
#include "torus.h"
#include "clipper.h"
#include "curve.h"
#include "plane.h"

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
    if (world->hit(r, 0.0001, FLT_MAX, rec))
    {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat->scatter(r, rec, attenuation, scattered))
        {
            return attenuation * colour(scattered, world, depth + 1);
        }
        else
            return vec3(0, 0, 0);
    }
    //Background colour
    
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1, 1, 1) + t * vec3(0.5, 0.7, 1);
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

/* Scripts for final */
//format Helpers
void spacer(int q)
{
    printf("--------------------------------------------------------------------\nQuestion: %d\n", q);
}


//Refraction Helpers
double transmission_angle_deg(double ni, double nt, double theta_i)
{
    double rads_i = theta_i * (M_PI / 180);
    double rads = asin((ni / nt) * (sin(rads_i)));
    return (rads * (180 / M_PI));
}

double transmission_angle_rad(double ni, double nt, double theta_i)
{
    return asin((ni / nt) * (sin(theta_i)));
}


//Box helpers
void print_intersects(const box_intersections& bi)
{
    std::cout << "Xmin: " << bi.x_intersections[0] << " Xmax: " << bi.x_intersections[1] << std::endl;
    std::cout << "Ymin: " << bi.y_intersections[0] << " Ymax: " << bi.y_intersections[1] << std::endl;
    //std::cout << "Zmin: " << bi.z_intersections[0] << " Zmax: " << bi.z_intersections[1] << std::endl;
}

//Gets intersection in x of next block over 
void increment_x(box_intersections& bi, const aabb& box)
{
    float x_range = box.end.x() - box.start.x();
    float y_range = bi.x_intersections[1].y() - bi.x_intersections[0].y();
    float z_range = box.end.z() - box.start.z();
    vec3 increment(x_range, y_range, z_range);
    bi.x_intersections[0] += increment;
    bi.x_intersections[1] += increment;

}

void increment_y(box_intersections& bi, const aabb& box)
{
    float x_range = bi.y_intersections[1].x() - bi.y_intersections[0].x();
    float y_range = box.end.y() - box.start.y();
    float z_range = box.end.z() - box.start.z();
    vec3 increment(x_range, y_range, z_range);
    bi.y_intersections[0] += increment;
    bi.y_intersections[1] += increment;
}
//TODO : handle z, also curious how z affects things hmmm, prob just slice by two const planes instead of just one.
void increment_z(box_intersections& bi)
{

}

vec3 refracted_vector(const vec3& n, const vec3& i, double ni, double nt)
{

    double incident_angle, refracted_angle, nr, discriminant;
    incident_angle = acos(dot(-i, n));
    nr = ni / nt;
    refracted_angle = asin(nr * sin(incident_angle));

    discriminant = 1.0 - pow(sin(refracted_angle), 2);
    if (discriminant < 0)
        return vec3();

    vec3 transmitted_ray = nr * i + (nr * cos(incident_angle) - sqrt(discriminant)) * n;
    transmitted_ray.make_unit_vector();
    return transmitted_ray;
}



int main() {
    printf("%f\n", transmission_angle_deg(1.0, 1.5, 65.1281));
    /* Script testing - refactor tonight
    printf("%f\n", transmission_angle_deg(1.000293, 1.5, 74.1));
    printf("%f\n", transmission_angle_deg(1.000293, 1.333, 48.5));

    //Testing if a ray hits a bounding box
    struct box_intersections intersects;
    aabb t1(vec3(20, 20, 0), vec3(40, 40, 0));
    ray r1(vec3(-14, 3.4, 0), vec3(0.766044, 0.642788, 0));
    if (t1.hit(r1, 0, 10000000, intersects))
        printf("1: That's a hit\n");

    aabb t2(vec3(40, 20, 0), vec3(60, 40, 0));
    ray r2(vec3(-14, 3.4, 0), vec3(0.819152, 0.573577, 0));
    if (t2.hit(r2, 0, 10000000, intersects))
        printf("2: That's a hit\n");

    aabb t3(vec3(20, 40, 0), vec3(40, 60, 0));
    ray r3(vec3(-14, 56.7146, 0), vec3(0.80358, -0.595197, 0));
    if (t3.hit(r3, 0, 10000000, intersects))
        printf("3: That's a hit\n");

    aabb t4(vec3(40, 20, 0), vec3(60, 40, 0));
    ray r4(vec3(45.1113, 37.2183, 0), vec3(-0.6944, -0.719589, 0));
    if (t4.hit(r4, 0, 10000000, intersects))
        printf("4: That's a hit\n");

    aabb t5(vec3(0, 0, 0), vec3(20, 20, 0));
    ray r5(vec3(-5.6, -7.1, 0), vec3(0.842403, 0.538849, 0));
    if (t5.hit(r5, 0, 10000000, intersects))
        printf("5: That's a hit\n");

    print_intersects(intersects);
    spacer(6);
    for (int i = 0; i < 3; i++)
    {
        increment_x(intersects, t5);
        print_intersects(intersects);
        spacer(0);
    }
    spacer(7);
    increment_y(intersects, t5);
    print_intersects(intersects);
    */
    
    
    /*Building a triangle
    vec3 vertices[3];
    
    vec3 a = vec3(0.5, 0.0, -1);
    vec3 b = vec3(0.75, 0.70, -1);
    vec3 c = vec3(1.0, 0.0, -1);
    */
    
    /*Build out the BVH
    hitable* root = new bvh_node(node_world, 0, 3);
    */

    /*
    //Building a cube, setup:
    
    hitable* list[1];
    cube * c1 = new cube(new lambertian(vec3(0.8, 0.3, 0.3)));
    c1->rotate_cube_x(30.0f);
    c1->rotate_cube_z(70.0f);
    list[0] = c1;
    hitable_list* world = new hitable_list(list, 1);
    */
    /*
    //Visualize triangle within the canonical cube
    hitable* list[2];
    cube* c1 = new cube(new lambertian(vec3(0.8, 0.3, 0.3)));

    vec3 a = vec3(0.5, -0.8, 0.6);
    vec3 b = vec3(0.6, 3, -0.7);
    vec3 c = vec3(-0.7, -0.5, -0.5);
    triangle * t1 = new triangle(a, b, c, new lambertian(vec3(0.0, 0.0, 0.5)));
    list[0] = c1;
    list[1] = t1;
    hitable_list* world = new hitable_list(list, 2);
    */
    /*
    //Clipping Setup
    vec3 a = vec3(0.5, -0.8, 0.6);
    vec3 b = vec3(0.6, 3, -0.7);
    vec3 c = vec3(-0.7, -0.5, -0.5);
    vec3 n = vec3(0, -1, 0); //Use above code to know how to call clipper and which face to use!
    //Order matters!!!
    vec3 i1 = compute_edge_intersection(a, b, n);
    vec3 i2 = compute_edge_intersection(c, b, n);
    shortest_edge(a, c, i1, i2);
    //Case of no intersection, incorrect results, always check output!!
    //vec3 i3 = compute_edge_intersection(a, c, n);
    
    //Getting interpolated vals
    vec3 col_a(255, 0, 0), norm_a(-0.087, -0.054, 0.995);
    vec3 col_b(125, 23, 54), norm_b(-0.426, 0.723, 0.542);
    interpolate_vals_linear(a, i1, b, col_a, col_b);
    interpolate_vals_linear(a, i1, b, norm_a, norm_b);

    //Getting barycentric vals in 2D
    float a[2] = { 1, 0 };
    float b[2] = { 2, 1 };
    float c[2] = { 2, 0 };
    float p[2] = { 1, 1 };
    //determinant();

    //Getting texture coords
    //1. Use worl coords to get barycentric vals
    //2. texture_coord = vec2(dot(barys, u), dot(barys, v)) 
    */
    /*
    //Sphere ray intersection
    ray r(vec3(-60, -40, 22), vec3(0.806707, 0.564863, -0.173648));
    sphere s(vec3(0, 0, 0), 20, new lambertian(vec3(0.0, 0.0, 0.5)));
    hit_record rec;
    s.hit(r, 0.001, 10000000000000000000, rec);
    std::cout << rec.p << std::endl;
    */
    
    /*
    //Plane ray intersection
    ray r(vec3(19.7235, -2.78681, 57.1043), vec3(0, -0.5, -0.866025));
    vec3 n(0.0464349, -0.250748, 0.966938);
    vec3 p(-20, 30, 25);
    plane pl(n, p, new lambertian(vec3(0.0, 0.0, 0.5)));
    hit_record rec;
    pl.hit(r, 0.001, 10000000000000000000, rec);
    std::cout << rec.p << std::endl;
    */

    /*
    //Spline computation
    curve c = curve(vec3(19, 28, 0), vec3(-8, 29, 0), vec3(19, -2.5, 0), curve_type::SPLINE);
    c.print_handles();
    */
    /*
    vec3 P1(0, 4, 0), P2(4, 4, 0), P3(3, 0, 0), I(2.6, 4, 0);
    vec3 barys = barycentric_coords(P1, P2, P3, I);
    if (in_triangle(barys))
        printf("we is hits!\n");
    vec3 inter = barys.x() * vec3(255, 125, 0) + barys.y() * vec3(255, 125, 0) + barys.z() * vec3(125, 25, 255);
    std::cout << inter << std::endl;
    */
    /*
    //Ray-Line 
    ray r(vec3(-70, 20, 0), vec3(1, 0, 0));
    vec3 a(-40, 0, 0);
    vec3 b(0, 40, 0);
    vec3 i1 = ray_line(r, a, b);
    std::cout << "Intersection point: " << i1 << std::endl;
    */
    
    //Testing simple refraction
    
    vec3 t1, t2, t3;
    //t1 = refracted_vector(vec3(-0.38, 0.92, 0.0), vec3(1, 0, 0), 1.0, 1.5);
    //std::cout << "First vector: " << t1 << std::endl;

    //t2 = refracted_vector(-vec3(0.5801, -0.8146, 0.0), vec3(0.8705, -0.4921, 0.0), 1.5, 1.0);
    //std::cout << "Second vector: " << t2 << std::endl;
    
    //t3 = refracted_vector(vec3(-0.420591, 0.90725, 0), vec3(1, 0, 0), )

    /*
    //Torus Setup
    hitable* objects[1];
    vec3 n(-1, 1, 1);
    n.make_unit_vector();
    objects[0] = new torus(vec3(0, 0, -1), n, 1.0f, 0.3f, new lambertian(vec3(0.8, 0.3, 0.3)));
    hitable* world = new hitable_list(objects, 1);
    */
    //To compute dist to torus:
    //define a ray n all other params necessary params for hit funct.
    //call hit funct on the torus
    //dist to torus = r0 + rd_hat*rec.t OR (rec.p - r0).length 
    /*
    vec3 center(45.63, 0, 0);
    vec3 disk_n(0, 0, 1);
    float r_disk = 45.63, r_tube = 16.96;
    float k = dot((vec3(-30, 0, 37) - center), disk_n);
    vec3 p = vec3(-30, 0, 37) - (k * disk_n);
    vec3 pc = (p - center);
    pc.make_unit_vector();
    vec3 m = center + pc * r_disk;
    vec3 mr = m - vec3(-30, 0, 37);
    std::cout << mr.length() - r_tube;
    */
    /*
    //Standard World setup
    hitable* objects[3];
    objects[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
    objects[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    objects[2] = new sphere(vec3(2.0, 0, -1), 0.55, new lambertian(vec3(0.0, 0.2, 0.8)));
    hitable* world = new hitable_list(objects, 3);
    */
    /*
    //Standard render setup
    int nx = 200;
    int ny = 100;
    int ns = 20;

    //Cam setup
    vec3 lookfrom(2, 2, 8);
    vec3 lookat(0, 0, -1);
    float dist_to_focus = 10.0;
    float aperture = 0.1;
    //Convention from lec is upvector is <0, 0, 1> (for test Qs), convention for me is <0, 1, 0>
    camera cam(lookfrom, lookat, vec3(0, 1, 0), 30, float(nx) / float(ny), aperture, dist_to_focus, false);

    float u, v;
    ray r;
    vec3 col;

    FILE* fd;
    if (fopen_s(&fd, "out/test_torus.ppm", "w") != 0 || fd == NULL)
        exit(errno);

    if (_dup2(_fileno(fd), _fileno(stdout)) != 0)
        exit(errno);

    std::cout << "P3\n" << nx << " " << ny << "\n255\n";


    
    for (int j = ny - 1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            col = vec3(0, 0, 0);
            for (int s = 0; s < ns; s++)
            {
                u = float(i + random_double()) / float(nx);
                v = float(j + random_double()) / float(ny);

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
    
    fclose(fd);
    */
    return 0;
}