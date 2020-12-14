#pragma once
#include "vec3.h"

class ray
{
private:
    vec3 r0;
    vec3 rd;

public:
    ray() {}
    ray(const vec3& r0, const vec3& rd) : r0 { r0 }, rd{ rd } {}

    vec3 origin() const { return r0; }
    vec3 direction() const { return rd; }

    //Gets point along this ray at time = t.
    vec3 point_at_parameter(float t) const { return r0 + t * rd; }

    
};