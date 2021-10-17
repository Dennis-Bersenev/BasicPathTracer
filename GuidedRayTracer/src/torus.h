#pragma once
#include "hitable.h"

class torus : public hitable
{
public:
	vec3 center;
	vec3 disk_n;
	double r_tube, r_disk, R;
	material mat;

	torus() {}

	//n must be unit length, r1 is dist from center to medial axis, r2 is dist from medial axis to surface.
	torus(vec3 c, vec3 n, double r1, double r2, material mat) : center{ c }, disk_n{ n }, r_disk{ r1 }, r_tube{ r2 }, mat{ mat }
	{
		R = r1 + r2;
	}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec, material& closest_mat) const override;
	virtual bool bounding_box(aabb& box) const override;

	//Gets the normal to the surface at the given point of intersection.
	inline vec3 surface_norm(const vec3& I, const vec3& m) const { return (I - m) / r_tube; }

	//Computes the closest point on the torus to the origin of the given ray and updates the corresponding point on the medial axis.
	inline double distance(const ray& r, vec3& m) const
	{
		double k = dot((r.origin() - center), disk_n);
		vec3 p = r.origin() - (k * disk_n);
		vec3 pc = (p - center);
		pc.make_unit_vector();
		m = center + pc * r_disk;
		vec3 mr = m - r.origin();
		return mr.length() - r_tube;
	}

	//Gets maximum distance on surface of torus from origin of given ray
	inline double max_distance(const ray& r) const
	{
		vec3 d = (center - r.origin());
		d.make_unit_vector();
		return (r.origin() - (center + d*R)).length();
	}
};

//Via sphere tracing algo
bool torus::hit(const ray& r, double t_min, double t_max, hit_record& rec, material& closest_mat) const
{
	
	double dsf = 0.0f, eps = 0.0001, max_dist = max_distance(r), radius;
	vec3 m;
	ray trc(r.origin(), unit_vector(r.direction()));
	
	do
	{
		radius = distance(trc, m);
		if (radius < eps)
		{
			if (dsf < t_min || dsf > t_max)
				return false;

			rec.t = dsf;
			rec.p = trc.origin();
			rec.normal = surface_norm(rec.p, m);
			closest_mat = mat;
			return true;
		} 
		trc.r0 += trc.direction() * radius;
		dsf += radius;
	} while (dsf < max_dist);

	return false;
}

bool torus::bounding_box(aabb& box) const
{
	return true;
}