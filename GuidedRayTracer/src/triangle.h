#pragma once
#include "hitable.h"

class triangle : public hitable {
public:

    vec3 a, b, c, n;
	material* mat;
	const float EPSILON = 0.001;

    triangle() {}

	triangle(const vec3& a, const vec3& b, const vec3& c, material* m) : a{ a }, b{ b }, c{ c }, mat{ m }
	{
		vec3 normal = cross((b - a), (c - a));
		n = unit_vector(normal);
	}

    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;

	inline bool in_plane(const ray& r, float t_min, float t_max, float * t) const
	{
		float num, denom, t_temp;
		float eps = 0.0001;
		denom = dot(n, r.direction());
		if (-eps < denom && denom < eps)
			return false;

		num = dot(n, a - r.origin());
		t_temp = num / denom;
		
		if (t_temp < t_min || t_temp > t_max)
			return false;

		*t = t_temp;
		return true;
	}
    
	inline bool in_triangle(const vec3& I) const
	{
		float area, alpha, beta, gamma, eps, sum;
		eps = 0.0001;

		area = 0.5f * (cross((b - a), (c - a))).length();
		alpha = (0.5f / area) * (cross((b - I), (c - I))).length();
		beta = (0.5f / area) * (cross((c - I), (a - I))).length();
		gamma = (0.5f / area) * (cross((a - I), (b - I))).length();
		
		if (alpha < 0 || beta < 0 || gamma < 0)
			return false;

		sum = alpha + beta + gamma;
		return (1.0f - eps < sum&& sum < 1.0f + eps);
	}
};


bool triangle::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	
	float t_temp;
	if (in_plane(r, t_min, t_max, &t_temp))
	{
		vec3 I = r.point_at_parameter(t_temp);
		if (in_triangle(I))
		{
			rec.t = t_temp;
			rec.p = I;
			rec.normal = n;
			rec.mat = mat;
			return true;
		}
	}
	
	return false;
}
