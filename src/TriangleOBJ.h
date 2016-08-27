#pragma once
#include "Triangle.h"

namespace RayTracing
{
	class TriangleOBJ : public Triangle
	{
	public:
		TriangleOBJ(SimpleOBJ::Vec3f *pos, SimpleOBJ::Vec3f &color, SimpleOBJ::Vec3f& diffuse, SimpleOBJ::Vec3f& specular, SimpleOBJ::Vec3f& transmiss, SimpleOBJ::Vec3f& environmentR, float refractivity, float s, const std::string &name) :
		  Triangle(pos, color, diffuse, specular, transmiss, environmentR, refractivity, s, name) {}

		bool CrossRay(SimpleOBJ::Vec3f& Ls, SimpleOBJ::Vec3f& dir, SimpleOBJ::Vec3f& Le, SimpleOBJ::Vec3f& P, int &inside);
	};
}