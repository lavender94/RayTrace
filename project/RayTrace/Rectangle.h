#pragma once
#include "Object.h"
#include "Vec3f.h"
#include "MyMath.h"

namespace RayTracing
{
#define acc_rectangle 1e-5

	class Rectangle : public Object
	{
	public:
		Rectangle(SimpleOBJ::Vec3f &corner, SimpleOBJ::Vec3f &length, SimpleOBJ::Vec3f &width, SimpleOBJ::Vec3f &color, SimpleOBJ::Vec3f& diffuse, SimpleOBJ::Vec3f& specular, SimpleOBJ::Vec3f& transmiss, SimpleOBJ::Vec3f& environmentR, float refractivity, float s, const std::string &name);

		bool CrossRay(SimpleOBJ::Vec3f& Ls, SimpleOBJ::Vec3f& dir, SimpleOBJ::Vec3f& Le, SimpleOBJ::Vec3f& P, int &inside);
		int CrossRay(SimpleOBJ::Vec3f& Ls, SimpleOBJ::Vec3f& Le);
		bool InScene(SimpleOBJ::Vec3f& LFD, SimpleOBJ::Vec3f& RBU);

		SimpleOBJ::Vec3f NormalVector(SimpleOBJ::Vec3f &pos) { return upward; }
		SimpleOBJ::Vec3f Center() { return corner+length/2+width/2; }

		bool move(SimpleOBJ::Vec3f &direction)
		{
			corner += direction;
			return true;
		}
		bool moveTo(SimpleOBJ::Vec3f &center)
		{
			corner = center-length/2-width/2;
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
		void countFace() { D = MyMath::innerProduct(upward._p, corner._p); }
		bool _inscene(SimpleOBJ::Vec3f& LFD, SimpleOBJ::Vec3f& RBU, SimpleOBJ::Vec3f &pos)
		{
			return pos.x >= LFD.x && pos.x < RBU.x && pos.y >= LFD.y && pos.y < RBU.y && pos.z >= LFD.z && pos.z < RBU.z;
		}
		bool crossFace(SimpleOBJ::Vec3f &L1, SimpleOBJ::Vec3f &L2, SimpleOBJ::Vec3f &cor, SimpleOBJ::Vec3f &LFD, SimpleOBJ::Vec3f &RBU);

	private:
		SimpleOBJ::Vec3f corner, length, width, upward;
		float D;//Ax+By+Cz=D
		int flag;//0:x 1:y 2:z to disable in calc
	};
}