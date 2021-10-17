#pragma once
#include "hitable.h"

class plane : hitable
{
private:
	vec3 normal;
	vec3 point;
	material mat;

public:

	//Full Constructor
	plane(const vec3& n, const vec3& p, material mat) : normal{ unit_vector(n) }, point{ p }, mat{ mat } {}

	virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec, material& closest_mat) const override;
	virtual bool bounding_box(aabb& box) const override;

	inline vec3 get_normal() const
	{
		return normal;
	}
};


bool plane::hit(const ray& r, double tmin, double tmax, hit_record& rec, material& closest_mat) const
{
	vec3 n = get_normal();
	double num, denom, t_temp;
	double eps = 0.0001;
	denom = dot(n, r.direction());
	if (-eps < denom && denom < eps)
		return false;

	num = dot(n, point - r.origin());
	t_temp = num / denom;

	if (t_temp < tmin || t_temp > tmax)
		return false;

	rec.t = t_temp;
	rec.p = r.point_at_parameter(rec.t);
	rec.normal = n;
	closest_mat = mat;
	return true;
}

bool plane::bounding_box(aabb& box) const 
{
	return false;
}