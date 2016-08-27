#pragma once
#include "Vec3f.h"
#include "Object.h"
#include "RaySource.h"
#include "Ray.h"
#include "Texture.h"
#include <list>
#include <Windows.h>

namespace RayTracing
{
#define PIE 3.1415926
#define acc_coincide 1e-6
#define acc_minDistance 1e-4

	class Scene
	{
	public:
		static std::vector<RaySource> RaySources;
		static std::vector<Object*> AllObjects, WaitObjects;
		static SimpleOBJ::Vec3f __LFD, __RBU;
		static int Background, nextBackground;
		static double alpha;//(1-alpha)*back1+alpha*back2
		static std::vector<Texture*> textures;
		static float radius, radius_sqr;
		static SimpleOBJ::Vec3f getBackground(SimpleOBJ::Vec3f &pos, SimpleOBJ::Vec3f &direction);
		static void LoadTexture(char *TextureName);
		static const float minDistance;
		static const int maxObjects;

		Scene() : LFD(0,0,0), RBU(0,0,0), son(0), father(0), ID(0) {}
		Scene(float *LFD, float *RBU) : LFD(LFD[0], LFD[1], LFD[2]), RBU(RBU[0], RBU[1], RBU[2]), son(0), father(0), ID(0) {}
		Scene(SimpleOBJ::Vec3f &LFD, SimpleOBJ::Vec3f &RBU) : LFD(LFD), RBU(RBU), son(0), father(0), ID(0) {}
		~Scene() { if (son) delete[] son; }

		void Initialize(bool firstTime);
		void shrink();

		SimpleOBJ::Vec3f outPos(Ray&);
		bool cross(Ray& ray, SimpleOBJ::Vec3f& rs_pos, std::vector<Object*> *objs, bool &flag, SimpleOBJ::Vec3f &pos);//True return obj if flag=true end; False return outPos
		bool cross(Ray&, SimpleOBJ::Vec3f&, Object *&, int &);//True return crossPos obj if obj=0 outOfScene; False return outPos
		//std::vector<std::pair<RaySource*, SimpleOBJ::Vec3f> > *countRaySources(SimpleOBJ::Vec3f &);

		SimpleOBJ::Vec3f LFD, RBU, mid;//LeftFrontDown RightBackUp
		Scene *son, *father;
		int ID;
	protected:
		void initialize();

		std::list<Object*> Objects;

		friend DWORD WINAPI ThreadProcInit(LPVOID lpParam);
		friend class Movement;
	};
}