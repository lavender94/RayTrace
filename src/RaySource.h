#pragma once
#include "Vec3f.h"

namespace RayTracing
{
	class RaySource
	{
	public:
		//brightness: range 0-1
		RaySource(float *pos, float *color, float brightness = 1.0f) : pos(pos[0], pos[1], pos[2]), color(color[0]*brightness, color[1]*brightness, color[2]*brightness) {}
		//brightness: range 0-1
		RaySource(SimpleOBJ::Vec3f& pos, SimpleOBJ::Vec3f& color, float brightness = 1.0f) : pos(pos), color(color*brightness) {}

		SimpleOBJ::Vec3f pos, color;
	};
}