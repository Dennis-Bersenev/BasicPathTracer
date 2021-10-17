#pragma once
#include "vec3.h"

enum class curve_type
{
	LINEAR,
	QUADRATIC,
	CUBIC,
	SPLINE
};

/*This class will be responsible for general curve geometry and will handle things like linear interpolation, splines, etc.*/
class curve
{
private:
	vec3 p0, p1, p2, p3;
	curve_type ct;

public:
	//Constructs linear Bezier curve
	curve(const vec3& p0, const vec3& p1) : p0{ p0 }, p1{ p1 }, ct{ curve_type::LINEAR }
	{}

	//Constructs quadratic Bezier curve OR a Spline out of two implied cubiz bezier curves given 3 control points.
	curve(const vec3& p0, const vec3& p1, const vec3& p2, curve_type type) : p0{ p0 }, p1{ p1 }, p2{ p2 }, ct{ type }
	{}

	//Constructs cubic Bezier curve
	curve(const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3) : p0{ p0 }, p1{ p1 }, p2{ p2 }, p3{ p3 }, ct{ curve_type::CUBIC }
	{}

	//Displays handler points for THIS Spline 
	inline void print_handles()
	{
		vec3 L1, L2, R1, R2;
		R1 = ((1.0 / 6) * (p2 - p0)) + p1;
		L2 = ((-1.0 / 6) * (p2 - p0)) + p1;
		L1 = L2 - ((1.0 / 3) * (p1 - p0));
		R2 = R1 + ((1.0 / 3) * (p2 - p1));
		printf("R1: (%f, %f, %f)\n", R1.x(), R1.y(), R1.z());
		printf("R2: (%f, %f, %f)\n", R2.x(), R2.y(), R2.z());
		printf("L1: (%f, %f, %f)\n", L1.x(), L1.y(), L1.z());
		printf("L2: (%f, %f, %f)\n\n\n", L2.x(), L2.y(), L2.z());
	}

	//Gets the point on the curve given t
	//t must be in [0, 1]
	inline vec3 get_point(double t)
	{
		vec3 pt;
		switch (ct)
		{
		case curve_type::LINEAR:
			pt = p0 + t * (p1 - p0);
			break;
		case curve_type::QUADRATIC:
			pt = pow((1 - t), 2) * p0 + 2.0 * (1 - t) * t * p1 + pow(t, 2) * p2;
			break;
		case curve_type::CUBIC:
			pt = pow(1 - t, 3) * p0 + 3.0 * t * pow(1 - t, 2) * p1 + 3.0 * pow(t, 2) * (1.0 - t) * p2 + pow(t, 3) * p3;
			break;
		case curve_type::SPLINE:
			//TODO (quite involved, after exam)
			pt = vec3();
			break;
		default:
			pt = vec3();
			break;
		}
		return pt;
	}

};

