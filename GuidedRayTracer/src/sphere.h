#pragma once
#include "hitable.h"

class sphere : public hitable {
public:

    vec3 center;
    double radius;
    material mat;

    sphere() {}
    sphere(vec3 cen, double r, material m) : center{ cen }, radius{ r }, mat{ m } {};

    virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec, material& closest_mat) const override;
    virtual bool bounding_box(aabb& box) const override;
};


bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec, material& closest_mat) const
{
    vec3 oc = r.origin() - center;
    double a = dot(r.direction(), r.direction());
    double b = dot(oc, r.direction());
    double c = dot(oc, oc) - radius * radius;
    double discriminant = b * b - a * c;
    if (discriminant > 0) {
        double temp = (-b - sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            closest_mat = mat;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            closest_mat = mat;
            return true;
        }
    }
    return false;
}

bool sphere::bounding_box(aabb& box) const
{
    box = aabb(center - vec3(radius, radius, radius),
               center + vec3(radius, radius, radius));
    return true;
}