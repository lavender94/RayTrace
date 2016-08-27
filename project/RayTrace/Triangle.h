#pragma once
#include "Vec3f.h"
#include "Object.h"
#include "MyMath.h"

namespace RayTracing
{
#define acc_triangle 1e-5

	class Triangle : public Object
	{
	public:
		Triangle(SimpleOBJ::Vec3f *pos, SimpleOBJ::Vec3f &color, SimpleOBJ::Vec3f& diffuse, SimpleOBJ::Vec3f& specular, SimpleOBJ::Vec3f& transmiss, SimpleOBJ::Vec3f& environmentR, float refractivity, float s, const std::string &name);

		bool CrossRay(SimpleOBJ::Vec3f& Ls, SimpleOBJ::Vec3f& dir, SimpleOBJ::Vec3f& Le, SimpleOBJ::Vec3f& P, int &inside);
		int CrossRay(SimpleOBJ::Vec3f& Ls, SimpleOBJ::Vec3f& Le);
		bool InScene(SimpleOBJ::Vec3f& LFD, SimpleOBJ::Vec3f& RBU);

		SimpleOBJ::Vec3f NormalVector(SimpleOBJ::Vec3f &pos) { return upward; }
		SimpleOBJ::Vec3f Center() { return (pos[0]+pos[1]+pos[2])/3; }

		bool move(SimpleOBJ::Vec3f &direction)
		{
			pos[0] += direction;
			pos[1] += direction;
			pos[2] += direction;
			return true;
		}
		bool moveTo(SimpleOBJ::Vec3f &center)
		{
			SimpleOBJ::Vec3f L = center-(pos[0]+pos[1]+pos[2])/3;
			for (int i=0; i<3; ++i)
				pos[i] += L;
			return true;
		}
		bool rotate(SimpleOBJ::Vec3f &center, SimpleOBJ::Vec3f &axis, double degree);
		bool rotate(float degree);
		bool upDown(float degree);
		bool leftRight(float degree);
		bool rotateTo(SimpleOBJ::Vec3f &forward, SimpleOBJ::Vec3f &upward);

	protected:
		void countFlag()
		{
			float max = -2;
			for (int i=0; i<3; ++i)
				if (fabs(upward[i]) > max)
				{
					max = fabs(upward[i]);
					flag = i;
				}
		}
		void countFace() { D = MyMath::innerProduct(upward._p, pos[0]._p); }
		bool _inscene(SimpleOBJ::Vec3f& LFD, SimpleOBJ::Vec3f& RBU, SimpleOBJ::Vec3f &pos)
		{
			return pos.x >= LFD.x && pos.x < RBU.x && pos.y >= LFD.y && pos.y < RBU.y && pos.z >= LFD.z && pos.z < RBU.z;
		}
		bool crossFace(SimpleOBJ::Vec3f &L1, SimpleOBJ::Vec3f &L2, SimpleOBJ::Vec3f &cor, SimpleOBJ::Vec3f &LFD, SimpleOBJ::Vec3f &RBU);

		SimpleOBJ::Vec3f pos[3], upward, L01, L02;
		float D;//ax+by+cz=D
		int flag;//0:x 1:y 2:z to disable in calc
	};
}