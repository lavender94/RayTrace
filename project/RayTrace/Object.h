#pragma once
#include <string>
#include <set>
#include "Vec3f.h"
#include "Ray.h"

namespace RayTracing
{
	class Scene;

	class Object
	{
	public:
		Object(float *color, SimpleOBJ::Vec3f& diffuse, SimpleOBJ::Vec3f& specular, SimpleOBJ::Vec3f& transmiss, SimpleOBJ::Vec3f& environmentR, float refractivity, float s, const char *name)
			: color(color[0], color[1], color[2]), diffuse(diffuse), specular(specular), Transmiss(transmiss), environmentR(environmentR), refractivity(refractivity), S(s), name(name) {}
		Object(SimpleOBJ::Vec3f &color, SimpleOBJ::Vec3f& diffuse, SimpleOBJ::Vec3f& specular, SimpleOBJ::Vec3f& transmiss, SimpleOBJ::Vec3f& environmentR, float refractivity, float s, const std::string &name)
			: color(color), diffuse(diffuse), specular(specular), Transmiss(transmiss), environmentR(environmentR), refractivity(refractivity), S(s), name(name) {}

		virtual bool CrossRay(SimpleOBJ::Vec3f& Ls, SimpleOBJ::Vec3f& dir, SimpleOBJ::Vec3f& Le, SimpleOBJ::Vec3f& P, int &inside) = 0;//(lightStart, direction, lightEnd, crossPos)
		virtual int CrossRay(SimpleOBJ::Vec3f& Ls, SimpleOBJ::Vec3f& Le) = 0;//do not count crossPos
		virtual bool InScene(SimpleOBJ::Vec3f& LFD, SimpleOBJ::Vec3f& RBU) = 0;//[LeftFrontDown, RightBackUp)

		virtual SimpleOBJ::Vec3f NormalVector(SimpleOBJ::Vec3f &pos) = 0;//return the normal vector at pos
		virtual SimpleOBJ::Vec3f Center() = 0;//return the center

		virtual SimpleOBJ::Vec3f getColor(SimpleOBJ::Vec3f &pos) { return color; }
		virtual SimpleOBJ::Vec3f getDiffuse(SimpleOBJ::Vec3f &pos) { return diffuse; }
		virtual SimpleOBJ::Vec3f getSpecular(SimpleOBJ::Vec3f &pos) { return specular; }
		virtual SimpleOBJ::Vec3f getEnvironmentReflect(SimpleOBJ::Vec3f &pos) { return environmentR; }
		virtual float getRefractivity(SimpleOBJ::Vec3f &pos) { return refractivity; }//if refractivity.k <= 1e-6 no refractance
		virtual SimpleOBJ::Vec3f getTransmiss(SimpleOBJ::Vec3f &pos) { return Transmiss; }
		virtual SimpleOBJ::Vec3f getTransmiss() { return Transmiss; }
		virtual float getS(SimpleOBJ::Vec3f &pos) { return S; }//高光指数

		virtual bool move(SimpleOBJ::Vec3f &direction) { return false; }
		virtual bool moveTo(SimpleOBJ::Vec3f &center) { return false; }
		virtual bool rotate(SimpleOBJ::Vec3f &center, SimpleOBJ::Vec3f &axis, double degree) { return false; }
		virtual bool rotate(float degree) { return false; }//clockwise
		virtual bool upDown(float degree) { return false; }//up>0
		virtual bool leftRight(float degree) { return false; }//left>0
		virtual bool rotateTo(SimpleOBJ::Vec3f &forward, SimpleOBJ::Vec3f &upward) { return false; }
		const std::string &getName() { return name; }

		std::set<Scene*> involvedScene;

	protected:
		SimpleOBJ::Vec3f color, diffuse, specular, Transmiss, environmentR;
		float refractivity, S;

		const std::string name;
	};
}