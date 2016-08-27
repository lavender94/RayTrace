#pragma once
#include <assert.h>
#include "Vec3f.h"

namespace RayTracing
{
	class Texture
	{
	public:
		Texture(): width(-1), height(-1), texture(0) {}
		Texture(char *textureName): width(-1), height(-1), texture(0) { Load(textureName); }
		bool isLoaded() { return (texture != 0); }
		void Load(char *fileName);

		int width, height;
		SimpleOBJ::Vec3f **texture;

		__forceinline SimpleOBJ::Vec3f* operator [](int height)
		{
			assert(height>=0 && height<this->height);
			return texture[height];
		}
		__forceinline const SimpleOBJ::Vec3f* operator [](int height) const
		{
			assert(height>=0 && height<this->height);
			return texture[height];
		}
	};
}