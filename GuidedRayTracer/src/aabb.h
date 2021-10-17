#pragma once
#include "ray.h"

struct box_intersections {
	vec3 x_intersections[2];
	vec3 y_intersections[2];
	vec3 z_intersections[2];
};
//Axis-aligned Bounding Box 
class aabb
{
private:
	// Keep track of smallest/largest coords in each dimension defined with the max, eg start.x <= x <= end.x 
	// i.e. coords of opp corners
	vec3 start, end;
public:
	
	aabb() {}
	aabb(const vec3& a, const vec3& b) : start{ a }, end{ b } {}

	/**
	* Function responsible for handling intersections between a ray and an aabb in the scene.
	* @param r - the ray intersecting this object.
	* @param t_min - the minimum distance along the ray for which a hit is valid (to prevent self-intersections).
	* @param t_max - the maximum distance along the ray for which a hit is valid (to prevent intersections further than closest_so_far
					 from updating the hit_record, i.e. pass in t of nearest intersection in for t_max)
	* @param rec - the hit_record for the ray's closest intersection point.
	* @return true if this aabb intersects the ray
	*/
	inline bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const
	{
		for (int i = 0; i < 3; i++)
		{
			double inv_denom = 1.0f / r.direction()[i];
			double t0 = (start[i] - r.origin()[i]) * inv_denom;
			double t1 = (end[i] - r.origin()[i]) * inv_denom;

			if (inv_denom < 0.0f)
				std::swap(t0, t1);
			
			t_min = (t0 > t_min) ? t0 : t_min;
			t_max = (t1 < t_max) ? t1 : t_max;

			if (t_min >= t_max)
				return false;
		}
		return true;
	}

	//Getters
	inline vec3 min() const { return start; }
	inline vec3 max() const { return end; }


};

/**
* Creates a box to enclose two boxes.
* @param box0/1 - the boxes to enclose.
* @return the enclosing box.
*/
inline aabb enclose_boxes(aabb box0, aabb box1) {
	vec3 small(fmin(box0.min().x(), box1.min().x()),
			   fmin(box0.min().y(), box1.min().y()),
			   fmin(box0.min().z(), box1.min().z()));

	vec3 big(fmax(box0.max().x(), box1.max().x()),
			 fmax(box0.max().y(), box1.max().y()),
			 fmax(box0.max().z(), box1.max().z()));

	return aabb(small, big);
}