#pragma once
#include "Vec3f.h"
#include "Camera.h"
#include "Movement.h"
#include "Sphere.h"

namespace RayTracing
{
	class CameraMove
	{
	public:
		CameraMove(Camera *camera) : camera(camera),
			p_v(camera->pos, 0.1f, SimpleOBJ::Vec3f(0,0,0), SimpleOBJ::Vec3f(0,0,0), SimpleOBJ::Vec3f(0,0,0), SimpleOBJ::Vec3f(0,0,0), SimpleOBJ::Vec3f(0,0,0), -1, 0, "p_v"),
			p_f(camera->pos+camera->forward*camera->focus, 0.1f, SimpleOBJ::Vec3f(0,0,0), SimpleOBJ::Vec3f(0,0,0), SimpleOBJ::Vec3f(0,0,0), SimpleOBJ::Vec3f(0,0,0), SimpleOBJ::Vec3f(0,0,0), -1, 0, "p_f"),
			p_u(camera->pos+camera->upward, 0.1f, SimpleOBJ::Vec3f(0,0,0), SimpleOBJ::Vec3f(0,0,0), SimpleOBJ::Vec3f(0,0,0), SimpleOBJ::Vec3f(0,0,0), SimpleOBJ::Vec3f(0,0,0), -1, 0, "p_u"),
			v(&p_v, false), f(&p_f, false), u(&p_u, false) {}

		void init();
		bool move();

	protected:
		Camera *camera;
		Sphere p_v,p_f,p_u;//visit forward upward point
		Movement v,f,u;

		friend class Movie;
	};
}