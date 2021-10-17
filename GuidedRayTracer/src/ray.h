#pragma once
#include "vec3.h"

struct hit_record
{
    //t parameter along the ray of the most recent intersection (i.e. time traveled by ray to reach this object)
    double t;
    //Hit point
    vec3 p;
    //Normal to surface at point of intersection
    vec3 normal;
};

class ray
{
private:
    vec3 rd;

public:
    vec3 r0;
    
    ray() {}
    ray(const vec3& r0, const vec3& rd) : r0 { r0 }, rd{ rd } {}

    vec3 origin() const { return r0; }
    vec3 direction() const { return rd; }

    //Gets point along this ray at time = t.
    vec3 point_at_parameter(double t) const { return r0 + t * rd; }

    
};