#pragma once

#include "ray.h"
#include "aabb.h"

class material;

struct hit_record
{
    //t parameter along the ray of the most recent intersection (i.e. time traveled by ray to reach this object)
    float t;
    //Hit point
    vec3 p;
    //Normal to surface at point of intersection
    vec3 normal;
    //Material type of object of closest intersection.
    material* mat;
};

class hitable {
public:
    /**
    * Function responsible for handling intersections between a ray and objects in the scene.
    * @param r - the ray intersecting this object.
    * @param t_min - the minimum distance along the ray for which a hit is valid (to prevent self-intersections).
    * @param t_max - the maximum distance along the ray for which a hit is valid (to prevent intersections further than closest_so_far
                     from updating the hit_record, i.e. pass in t of nearest intersection in for t_max)
    * @param rec - the hit_record for the ray's closest intersection point.
    * @return true if this object intersects the ray 
    */
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;

    /**
    * Encloses the object within an axis-aligned bounding box.
    * @param box - the box to enclose this object.
    * @return true if the object was enclosed (not all geometry can be bound, eg. infinite plane).
    */
    virtual bool bounding_box(aabb& box) const = 0;
};