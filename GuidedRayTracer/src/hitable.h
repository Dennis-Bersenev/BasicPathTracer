#pragma once

#include "aabb.h"
#include "material.h"
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
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec, material& closest_mat) const = 0;

    /**
    * Encloses the object within an axis-aligned bounding box.
    * @param box - the box to enclose this object.
    * @return true if the object was enclosed (not all geometry can be bound, eg. infinite plane).
    */
    virtual bool bounding_box(aabb& box) const = 0;

    virtual ~hitable() {}
};