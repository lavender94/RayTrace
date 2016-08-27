#pragma once
#include "Object.h"

namespace RayTracing
{
#define acc_sphere 1.5e-3

	class Sphere : public Object
	{
	public:
		Sphere(float *center, float radius, float *color, SimpleOBJ::Vec3f& diffuse, SimpleOBJ::Vec3f& specular, SimpleOBJ::Vec3f& transmiss, SimpleOBJ::Vec3f& environmentR, float refractivity, float s, const char *name) :
		  center(center[0], center[1], center[2]), radius(radius), r_sqr(radius*radius), Object(color, diffuse, specular, transmiss, environmentR, refractivity, s, name) {}
		Sphere(SimpleOBJ::Vec3f &center, float radius, SimpleOBJ::Vec3f &color, SimpleOBJ::Vec3f& diffuse, SimpleOBJ::Vec3f& specular, SimpleOBJ::Vec3f& transmiss, SimpleOBJ::Vec3f& environmentR, float refractivity, float s, const std::string &name) :
		  center(center), radius(radius), r_sqr(radius*radius), Object(color, diffuse, specular, transmiss, environmentR, refractivity, s, name) {}

		bool CrossRay(SimpleOBJ::Vec3f& Ls, SimpleOBJ::Vec3f& dir, SimpleOBJ::Vec3f& Le, SimpleOBJ::Vec3f& P, int &inside);//(lightStart, direction, lightEnd, crossPos)
		int CrossRay(SimpleOBJ::Vec3f& Ls, SimpleOBJ::Vec3f& Le);//do not count crossPos
		bool InScene(SimpleOBJ::Vec3f& LFD, SimpleOBJ::Vec3f& RBU);//(LeftFrontDown, RightBackUp)

		SimpleOBJ::Vec3f NormalVector(SimpleOBJ::Vec3f &pos)
		{
			SimpleOBJ::Vec3f n = pos - center;
			n.Normalize();
			return n;
		}
		SimpleOBJ::Vec3f Center() { return center; }

		bool move(SimpleOBJ::Vec3f &direction)
		{
			center += direction;
			return true;
		}
		bool moveTo(SimpleOBJ::Vec3f &center)
		{
			this->center = center;
			return true;
		}
		bool rotate(SimpleOBJ::Vec3f &center, SimpleOBJ::Vec3f &axis, double degree);

	private:
		SimpleOBJ::Vec3f center;
		float radius, r_sqr;
	};
}