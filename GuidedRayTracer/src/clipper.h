#pragma once
#include "vec3.h"
#include <iostream>
#include <stdio.h>
//Assumes we never get case where edges intersect diff faces 

/**
* Computes and prints the point where the edge connecting vertices v1 and v2 intersects the face of the canonical view volume.
* @param v1/v1 - v1 is inside the vol, v2 is outside the vol, i.e. its on the caller to know which is which!
* @param n - the inward facing (unit) surface norm of one of the faces
* @return the point of intersection (will also print to console).
*/
vec3 compute_edge_intersection(vec3 v1, vec3 v2, vec3 n)
{
	float d1, d2, t1;
	vec3 p = -n, res;
	d1 = dot(n, (v1 - p));
	d2 = dot(n, (v2 - p));
	t1 = d1 / (d1 - d2);
	res = (v1 + t1 * (v2 - v1));
	std::cout << "Point of intersection: " << res << std::endl;
	return res;
}

/**
* Prints the shortest edge to console, used to re-triangulate, only when two vertices are inside.
* @param v1 - the vertex of edge intersecting at i1
* @param v2 - the vertex of edge intersecting at i2
* @param i1/i2 - intersection points
*/
void shortest_edge(vec3 v1, vec3 v2, vec3 i1, vec3 i2)
{
	float e1, e2;
	e1 = (i1 - v2).length();
	e2 = (i2 - v1).length();
	if (e1 > e2)
	{
		printf("Use edge from v1 to i2\n");
	}
	else 
	{
		printf("Use edge from v2 to i1\n");
	}
}

//Interpolate vals at the intersection point, given coords of all 3 pts, and vals of end pts to interpolate.
//Consider it a line from v to u and i is a pt on the line
vec3 interpolate_vals_linear(vec3 v, vec3 i, vec3 u, vec3 v_val, vec3 u_val)
{
	float d1 = (i - v).length(), d2 = (u - i).length(), d = d1 + d2;
	vec3 res = (d1 / d) * u_val + (d2 / d) * v_val;
	std::cout << "interpolated value: " << res << std::endl;
	return res;
}

//This is a lot of messy work and needs too many arguments!
vec3 interpolate_vals_bilinear(const vec3& a, const vec3& b, const vec3& c, const vec3& p)
{
	vec3 e1 = p - a;
	vec3 e2 = c - b;
	float u = ((e1.x() * a.y() + (e1.y() * (c.x() - a.x()))) - (e1.x() * c.y())) / (e2.y() * e1.x() - e1.y() * c.y());
	vec3 I = e2 * u + b;
	
	return I;
}

vec3 interpolate_vals_bary(const vec3& bary, const vec3& vals)
{
	return bary * vals;
}

//a, b, c being the vertices, and I being the point whose barycentric coords you wanna find
vec3 barycentric_coords(const vec3& a, const vec3& b, const vec3& c, const vec3& I)
{
	float area, alpha, beta, gamma;

	area = 0.5f * (cross((b - a), (c - a))).length();
	alpha = (0.5f / area) * (cross((b - I), (c - I))).length();
	beta = (0.5f / area) * (cross((c - I), (a - I))).length();
	gamma = (0.5f / area) * (cross((a - I), (b - I))).length();
	
	vec3 res = vec3(alpha, beta, gamma);
	std::cout << "barycentric values: " << res << std::endl;
	return res;
}


//Specified vector are the barycentric coords
bool in_triangle(const vec3& bary) 
{
	float eps = 0.0001;
	if (bary.x() < 0 || bary.y() < 0 || bary.z() < 0)
		return false;

	float sum = bary.x() + bary.y() + bary.z();
	return (1.0f - eps < sum && sum < 1.0f + eps);
}

vec3 ray_line(const ray& r, const vec3& a, const vec3& b)
{
	vec3 e1 = r.direction();
	vec3 e2 = a - b;
	float u = ((e1.x() * a.y() + (e1.y() * (a.x() - a.x()))) - (e1.x() * a.y())) / (e2.y() * e1.x() - e1.y() * a.y());
	vec3 I = e2 * u + b;

	return I;
}