#pragma once
#include "Vec3f.h"
#include "Ray.h"
#include "MyMath.h"
#include <cmath>
#include <vector>

namespace RayTracing
{
#define acc_direction 1e-6

	class Camera
	{
	public:
		Camera(int width, int height, float sceneWidth, float sceneHeight, SimpleOBJ::Vec3f &pos, SimpleOBJ::Vec3f &forward, SimpleOBJ::Vec3f &upward, float f = 1.0f) :
		  width(width), height(height), sceneWidth(sceneWidth), sceneHeight(sceneHeight), pos(pos), forward(forward), upward(upward), focus(f)
		{
			this->forward.Normalize();
			this->upward.Normalize();
			MyMath::crossProduct(this->upward._p, this->forward._p, y._p);
			if (fabs(MyMath::innerProduct(this->forward._p, this->upward._p)) > acc_direction)
				MyMath::crossProduct(this->forward._p, this->y._p, this->upward._p);
		}

		void move(SimpleOBJ::Vec3f direction)
		{
			pos += direction;
		}
		void move(float distance)
		{
			pos += forward*distance;
		}

		void set(SimpleOBJ::Vec3f &pos, SimpleOBJ::Vec3f &forward, SimpleOBJ::Vec3f &upward)
		{
			this->forward = forward;
			this->upward = upward;
			this->pos = pos;
			MyMath::crossProduct(this->upward._p, this->forward._p, y._p);
			if (fabs(MyMath::innerProduct(this->forward._p, this->upward._p)) > acc_direction)
				MyMath::crossProduct(this->forward._p, this->y._p, this->upward._p);
		}

		void upDown(float degree)//up>0
		{
			if (fabs(degree) < 1e-6) return;
			double sind = sin(degree), cosd = cos(degree);
			SimpleOBJ::Vec3f nU = upward*cosd - forward*sind,
				nF = forward*cosd + upward*sind;
			upward = nU;
			forward = nF;
		}

		void leftRight(float degree)//left>0
		{
			if (fabs(degree) < 1e-6) return;
			double sind = sin(degree), cosd = cos(degree);
			SimpleOBJ::Vec3f nF = forward*cosd + y*sind,
				nY = y*cosd - forward*sind;
			forward = nF;
			y = nY;
		}

		void rotate(float degree)//clockwise
		{
			if (fabs(degree) < 1e-6) return;
			double sind = sin(degree), cosd = cos(degree);
			SimpleOBJ::Vec3f nU = upward*cosd - y*sind,
				nY = y*cosd + forward*sind;
			upward = nU;
			y = nY;
		}
		std::vector<Ray>& getRay();
		std::vector<Ray>* getRay_antiAliasing();

		SimpleOBJ::Vec3f pos, forward, upward, y;
		float focus, sceneWidth, sceneHeight;
		int width, height;
	};
}