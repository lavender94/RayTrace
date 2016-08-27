#pragma once
#include <Windows.h>
#include <vector>
#include "Vec3f.h"
#include "Ray.h"
#include "Object.h"
#include "Camera.h"
#include "Scene.h"
#include "RaySource.h"

namespace RayTracing
{
#define acc_trace 1e-9
#define acc_transmiss 1e-6

	class RayTrace
	{
	public:
		RayTrace(Camera *camera, Scene *scene, int maxDepth = 2, bool smooth = false, bool antiAliasing = false) : camera(camera), scene(scene), max_depth(maxDepth), smooth(smooth), antiAliasing(antiAliasing) {}//s:高光指数
		SimpleOBJ::Vec3f BRDF(SimpleOBJ::Vec3f &pos, SimpleOBJ::Vec3f &direction, Object *obj);//phong N:normal vector
		std::vector<SimpleOBJ::Vec3f>& Trace();

		void setMaxDepth(int MaxDepth) { max_depth = MaxDepth; }

		Camera *camera;
		Scene *scene;

		bool smooth, antiAliasing;

	private:
		void Smooth(std::vector<SimpleOBJ::Vec3f> *&colors);

		SimpleOBJ::Vec3f rsTrace(Ray &ray, Scene *scene, RaySource &rs);
		SimpleOBJ::Vec3f trace(Ray &ray, Scene *scene, float n0, int depth);
		bool inScene(SimpleOBJ::Vec3f &pos, SimpleOBJ::Vec3f &direction, Scene *scene);//judge a point whether in scene
		Scene *subScene(SimpleOBJ::Vec3f &pos, SimpleOBJ::Vec3f &direction, Scene *scene);//find the smallest scene that contains pos

		bool Reflect(SimpleOBJ::Vec3f& pos, SimpleOBJ::Vec3f& direction, Object* obj, Ray &ray, int completeRef);
		int Refract(SimpleOBJ::Vec3f& pos, float n0, float n1, SimpleOBJ::Vec3f& direction, Object *obj, Ray &ray);

		int max_depth;

		friend DWORD WINAPI ThreadProc(LPVOID lpParam);//trace lpParam = vector<Ray>*
	};

	struct para_Threat
	{
		std::vector<Ray> *rays;
		std::vector<SimpleOBJ::Vec3f> *colors;
		RayTrace *rayTrace;
	};
}