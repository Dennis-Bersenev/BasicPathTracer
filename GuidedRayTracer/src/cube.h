#pragma once
#include "triangle.h"
#include "hitable_list.h"


//This whole class outta be cleaned up eventually!
class cube : public hitable
{
public:
	material mat; //Not sure if neccessary tbh.

	//Ideally read from file
	//Renders canonical view volume!
	vec3 vertices[8] = {
		vec3(-1, -1, 1), vec3(-1, 1, 1), vec3(1, 1, 1), vec3(1, -1, 1),
		vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, 1, -1), vec3(-1, -1, -1)
	};

	int indices[36] = {
		1, 0, 2,
		3, 2, 0,

		3, 5, 2,
		4, 2, 5,

		0, 7, 1,
		1, 6, 7,

		2, 4, 1,
		6, 1, 4,

		3, 0, 5,
		7, 5, 0,

		6, 4, 5,
		7, 6, 5

	};

	cube(material mat) : mat{ mat } {}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec, material& closest_mat) const;
	virtual bool bounding_box(aabb& box) const override;


	inline void build_cube(hitable ** triangles) const
	{
		//Once for each row of the order matrix (i.e. once for each triangle)
		int a, b, c;
		for (int i = 0; i < 12; i++)
		{
			a = indices[3 * i];
			b = indices[3 * i + 1];
			c = indices[3 * i + 2];
			triangles[i] = new triangle(vertices[a], vertices[b], vertices[c], mat);
		}
	}

	//Take angle in degrees and transform to rads 
	inline void rotate_cube_x(double theta)
	{
		double c, s;
		c = cos((theta / 180) * M_PI);
		s = sin((theta / 180) * M_PI);
		vec3 col1, col2, col3;
		col1 = vec3(1, 0, 0);
		col2 = vec3(0, c, s);
		col3 = vec3(0, -s, c);
		vec3 temp;
		for (int i = 0; i < 8; i++)
		{
			temp = vertices[i].x() * col1 + vertices[i].y() * col2 + vertices[i].z() * col3;
			vertices[i] = temp;
		}
	}

	inline void rotate_cube_y(double theta)
	{
		double c, s;
		c = cos((theta / 180) * M_PI);
		s = sin((theta / 180) * M_PI);
		vec3 col1, col2, col3;
		col1 = vec3(c, 0, -s);
		col2 = vec3(0, 1, 0);
		col3 = vec3(s, 0, c);
		vec3 temp;
		for (int i = 0; i < 8; i++)
		{
			temp = vertices[i].x() * col1 + vertices[i].y() * col2 + vertices[i].z() * col3;
			vertices[i] = temp;
		}
	}

	inline void rotate_cube_z(double theta)
	{
		double c, s;
		c = cos((theta / 180) * M_PI);
		s = sin((theta / 180) * M_PI);
		vec3 col1, col2, col3;
		col1 = vec3(c, s, 0);
		col2 = vec3(-s, c, 0);
		col3 = vec3(0, 0, 1);
		vec3 temp;
		for (int i = 0; i < 8; i++)
		{
			temp = vertices[i].x() * col1 + vertices[i].y() * col2 + vertices[i].z() * col3;
			vertices[i] = temp;
		}
	}
};

bool cube::hit(const ray& r, double t_min, double t_max, hit_record& rec, material& closest_mat) const
{
	hitable* triangles[12];
	build_cube(triangles);
	hitable* faces = new hitable_list(triangles, 12);
	bool res = faces->hit(r, t_min, t_max, rec, closest_mat);
	delete[] faces;
	return res;
}
//TODO
bool cube::bounding_box(aabb& box) const
{
	return false;
}