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
public:
	//Keep track of smallest/largest coords in each dimension defined with the max, eg start.x <= x <= end.x
	vec3 start, end;
	
	aabb() {}
	aabb(const vec3& a, const vec3& b) : start{ a }, end{ b } {}

	inline bool hit(const ray& r, float t_min, float t_max, box_intersections& bi) const
	{
		for (int i = 0; i < 3; i++)
		{
			float inv_denom = 1.0f / r.direction()[i];
			float t0 = (start[i] - r.origin()[i]) * inv_denom;
			float t1 = (end[i] - r.origin()[i]) * inv_denom;

			if (inv_denom < 0.0f)
				std::swap(t0, t1);
			
			t_min = (t0 > t_min) ? t0 : t_min;
			t_max = (t1 < t_max) ? t1 : t_max;

			if (t_min >= t_max)
				return false;

			//Messy code for computing intersection points, doesnt work properly.
			if (i == 0) {
				bi.x_intersections[0] = r.point_at_parameter(t0);
				bi.x_intersections[1] = r.point_at_parameter(t1);
			} else if(i == 1) {
				bi.y_intersections[0] = r.point_at_parameter(t0);
				bi.y_intersections[1] = r.point_at_parameter(t1);
			}
			else {
				bi.z_intersections[0] = r.point_at_parameter(t0);
				bi.z_intersections[1] = r.point_at_parameter(t1);
			}
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