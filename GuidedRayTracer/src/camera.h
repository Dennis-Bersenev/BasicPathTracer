#pragma once
#include "ray.h"

class camera {
public:

    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;

    camera(vec3 lookfrom, vec3 lookat, vec3 vup, double vfov, double aspect) {
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = unit_vector(cross(w, u));

        double theta = vfov * M_PI / 180;
        double height = tan(theta);
        double width = aspect * height;
        origin = lookfrom;

        lower_left_corner = origin - (width / 2) * u - (height / 2) * v - w;
        horizontal = width * u;
        vertical = height * v;
    }

    inline ray get_ray(double s, double t) {
        return ray(origin, lower_left_corner + s * horizontal + t * vertical - origin);
    }

};