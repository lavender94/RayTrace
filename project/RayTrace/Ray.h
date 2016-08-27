#pragma once
#include "Vec3f.h"

namespace RayTracing
{
	class Ray
	{
	public:
		Ray() {}
		Ray(const float *s, const float *dir) : start(s[0], s[1], s[2]), direction(dir[0], dir[1], dir[2]) {}
		Ray(SimpleOBJ::Vec3f& s, SimpleOBJ::Vec3f& dir) : start(s), direction(dir) {}

		SimpleOBJ::Vec3f start, direction;
	};
}