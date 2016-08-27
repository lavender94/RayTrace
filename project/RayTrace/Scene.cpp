#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cmath>
#include <Windows.h>
#include <time.h>
#include "Scene.h"
#include "MyMath.h"

using namespace std;

namespace RayTracing
{
	vector<RaySource> Scene::RaySources;
	vector<Object*> Scene::AllObjects, Scene::WaitObjects;
	SimpleOBJ::Vec3f Scene::__LFD, Scene::__RBU;
	int Scene::Background = -1, Scene::nextBackground = -1;;
	double Scene::alpha = 0;
	vector<Texture*> Scene::textures;
	float Scene::radius, Scene::radius_sqr;

	const float Scene::minDistance = 0.01f;
	const int Scene::maxObjects = 3;

	void Scene::LoadTexture(char *TextureName)
	{
		textures.push_back(new Texture(TextureName));
	}

	SimpleOBJ::Vec3f Scene::getBackground(SimpleOBJ::Vec3f &pos, SimpleOBJ::Vec3f &direction)
	{
		if (Background == -1)
			return SimpleOBJ::Vec3f(0,0,1);
		SimpleOBJ::Vec3f n = SimpleOBJ::Vec3f(50,50,0) - pos;
		float t = MyMath::innerProduct(direction._p, n._p);
		t += sqrt(radius_sqr - n.L2Norm_Sqr() + t*t);
		SimpleOBJ::Vec3f n_v = pos + direction*t;
		Texture &background = *textures[Background];
		double _h = 1-fabs(n_v.z)/sqrt(n_v.L2Norm_Sqr());
		if (n_v.y == 0 && n_v.x == 0)
			if (nextBackground != -1)
			{
				Texture &nextBack = *textures[nextBackground];
				return background[background.height-1][background.width/2]*(1-alpha) + nextBack[nextBack.height-1][nextBack.width/2]*alpha;
			}
			else
				return background[background.height-1][background.width/2];
		else
			//(x,y)->(y,-x)
			if (nextBackground != -1)
			{
				Texture &nextBack = *textures[nextBackground];
				double temp;
// 				if (n_v.x < 1e-6 && n_v.y < 1e-6)
// 					temp = 0.5;
// 				else
 					temp = (1+atan2(-(n_v.x), n_v.y)/PIE)/2;
				return background[int((background.height-1)*_h)][(int)((background.width-1)*temp)]*(1-alpha) + nextBack[int((nextBack.height-1)*_h)][(int)((nextBack.width-1)*temp)]*alpha;
			}
			else
// 				if (n_v.x < 1e-6 && n_v.y < 1e-6)
// 					return background[int((background.height-1)*_h)][(background.width-1)/2];
// 				else
					return background[int((background.height-1)*_h)][(int)((background.width-1)*(1+atan2(-(n_v.x), n_v.y)/PIE)/2)];
	}

	static DWORD WINAPI ThreadProcInit(LPVOID lpParam)
	{
		Scene *scene = (Scene *)lpParam;
		SimpleOBJ::Vec3f &LFD = scene->LFD, &RBU = scene->RBU, distance = RBU-LFD;
		list<Object*> &Objects = scene->Objects;
		float minDistance = Scene::minDistance;
		if (distance.x <= minDistance || distance.y <= minDistance || distance.z <= minDistance)
		{
			HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("__NoticeOBJ__"));
			WaitForSingleObject(hMutex, INFINITE);
			for (list<Object*>::iterator iter = Objects.begin(); iter != Objects.end(); ++iter)
				(*iter)->involvedScene.insert(scene);
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			printf(".");
			return 0;
		}
		int maxObjects = Scene::maxObjects;
		Scene *&son = scene->son;
		if (son == 0 && Objects.size() < maxObjects)
		{
			HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("__NoticeOBJ__"));
			WaitForSingleObject(hMutex, INFINITE);
			for (list<Object*>::iterator iter = Objects.begin(); iter != Objects.end(); ++iter)
				(*iter)->involvedScene.insert(scene);
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			printf(".");
			return 0;
		}
		
		if (son == 0)
		{
			son = new Scene[8];
			for (int i=0; i<8; ++i)
			{
				son[i].father = scene;
				son[i].ID = i;
			}
			SimpleOBJ::Vec3f &mid = scene->mid;
			mid[0] = mid[1] = mid[2] = 0;
			int s = Objects.size();
			for (list<Object*>::iterator iter = Objects.begin(); iter != Objects.end(); ++iter)
				mid += (*iter)->Center()/s;
			son[0].LFD = LFD;
			son[0].RBU = mid;
			son[1].LFD = SimpleOBJ::Vec3f(mid.x, LFD.y, LFD.z);
			son[1].RBU = SimpleOBJ::Vec3f(RBU.x, mid.y, mid.z);
			son[2].LFD = SimpleOBJ::Vec3f(LFD.x, mid.y, LFD.z);
			son[2].RBU = SimpleOBJ::Vec3f(mid.x, RBU.y, mid.z);
			son[3].LFD = SimpleOBJ::Vec3f(mid.x, mid.y, LFD.z);
			son[3].RBU = SimpleOBJ::Vec3f(RBU.x, RBU.y, mid.z);
			son[4].LFD = SimpleOBJ::Vec3f(LFD.x, LFD.y, mid.z);
			son[4].RBU = SimpleOBJ::Vec3f(mid.x, mid.y, RBU.z);
			son[5].LFD = SimpleOBJ::Vec3f(mid.x, LFD.y, mid.z);
			son[5].RBU = SimpleOBJ::Vec3f(RBU.x, mid.y, RBU.z);
			son[6].LFD = SimpleOBJ::Vec3f(LFD.x, mid.y, mid.z);
			son[6].RBU = SimpleOBJ::Vec3f(mid.x, RBU.y, RBU.z);
			son[7].LFD = mid;
			son[7].RBU = RBU;
		}

		for (int i=0; i<8; ++i)
		{
			SimpleOBJ::Vec3f slfd = son[i].LFD, srbu = son[i].RBU;
			for (list<Object*>::iterator iter = Objects.begin(); iter != Objects.end(); ++iter)
				if ((*iter)->InScene(slfd, srbu))
				{
					son[i].Objects.push_back(*iter);
					if ((son[i].Objects.size()>>3) > maxObjects)
						son[i].initialize();
				}
				son[i].initialize();
		}

		Objects.clear();
		printf(".");
		return 0;
	}

	void Scene::Initialize(bool firstTime)
	{
		printf("Initializing");
		clock_t s_t = clock();
		if (firstTime)
		{
			WaitObjects.clear();
			AllObjects.shrink_to_fit();
			__LFD = LFD;
			__RBU = RBU;
			SimpleOBJ::Vec3f temp = __RBU-__LFD;
			temp.x /= 2;
			temp.y /= 2;
			radius_sqr = temp.L2Norm_Sqr();
			radius = sqrt(radius_sqr);
		}
		vector<Object*> &AObj = firstTime ? AllObjects : WaitObjects;
		SimpleOBJ::Vec3f distance = RBU-LFD;
		if (distance.x <= minDistance || distance.y <= minDistance || distance.z <= minDistance)
		{
			for (vector<Object*>::iterator iter = AObj.begin(); iter != AObj.end(); ++iter)
			{
				Objects.push_back(*iter);
				(*iter)->involvedScene.insert(this);
			}
			WaitObjects.clear();
			return;
		}
		if ((firstTime && AObj.size() < maxObjects) || (!firstTime && son == 0 && AObj.size() + Objects.size() < maxObjects))
			for (vector<Object*>::iterator iter = AObj.begin(); iter != AObj.end(); ++iter)
			{
				Objects.push_back(*iter);
				(*iter)->involvedScene.insert(this);
			}
		else
		{
			if (son == 0)
			{
				son = new Scene[8];
				for (int i=0; i<8; ++i)
				{
					son[i].father = this;
					son[i].ID = i;
				}
				mid[0] = mid[1] = mid[2] = 0;
				int s = AObj.size();
				for (vector<Object*>::iterator iter = AObj.begin(); iter != AObj.end(); ++iter)
					mid += (*iter)->Center()/s;
				bool okey = true;
				for (int i=0; i<3; ++i)
					if ((mid[i]-LFD[i]<minDistance*200) || (RBU[i]-mid[i]<minDistance*200))
					{
						okey = false;
						break;
					}
				if (okey == false)
					mid = (LFD+RBU)/2;
				son[0].LFD = LFD;
				son[0].RBU = mid;
				son[1].LFD = SimpleOBJ::Vec3f(mid.x, LFD.y, LFD.z);
				son[1].RBU = SimpleOBJ::Vec3f(RBU.x, mid.y, mid.z);
				son[2].LFD = SimpleOBJ::Vec3f(LFD.x, mid.y, LFD.z);
				son[2].RBU = SimpleOBJ::Vec3f(mid.x, RBU.y, mid.z);
				son[3].LFD = SimpleOBJ::Vec3f(mid.x, mid.y, LFD.z);
				son[3].RBU = SimpleOBJ::Vec3f(RBU.x, RBU.y, mid.z);
				son[4].LFD = SimpleOBJ::Vec3f(LFD.x, LFD.y, mid.z);
				son[4].RBU = SimpleOBJ::Vec3f(mid.x, mid.y, RBU.z);
				son[5].LFD = SimpleOBJ::Vec3f(mid.x, LFD.y, mid.z);
				son[5].RBU = SimpleOBJ::Vec3f(RBU.x, mid.y, RBU.z);
				son[6].LFD = SimpleOBJ::Vec3f(LFD.x, mid.y, mid.z);
				son[6].RBU = SimpleOBJ::Vec3f(mid.x, RBU.y, RBU.z);
				son[7].LFD = mid;
				son[7].RBU = RBU;
			}

 			HANDLE hThread[8], hMutex = CreateMutex(NULL, false, TEXT("__NoticeOBJ__"));
			if (hMutex == NULL)
			{
				printf("Create mutex error!\n");
				return;
			}

			for (int i=0; i<8; ++i)
			{
				SimpleOBJ::Vec3f slfd = son[i].LFD, srbu = son[i].RBU;
				for (vector<Object*>::iterator iter = AObj.begin(); iter != AObj.end(); ++iter)
					if ((*iter)->InScene(slfd, srbu))
						son[i].Objects.push_back(*iter);
				hThread[i] = CreateThread(NULL, 0, ThreadProcInit, (LPVOID)(son+i), 0, 0);
			}

			WaitForMultipleObjects(8, hThread, true, INFINITE);
			for (int i=0; i<8; ++i)
				CloseHandle(hThread[i]);
			CloseHandle(hMutex);
		}
		WaitObjects.clear();
		printf("End\n%f sec\n", (float)(clock()-s_t)/CLOCKS_PER_SEC);
	}

	void Scene::initialize()
	{
		if (son == 0 && Objects.size() <= maxObjects)
		{
			HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("__NoticeOBJ__"));
			WaitForSingleObject(hMutex, INFINITE);
			for (list<Object*>::iterator iter = Objects.begin(); iter != Objects.end(); ++iter)
				(*iter)->involvedScene.insert(this);
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			return;
		}
		SimpleOBJ::Vec3f distance = RBU-LFD;
		if (distance.x <= minDistance || distance.y <= minDistance || distance.z <= minDistance)
		{
			HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("__NoticeOBJ__"));
			WaitForSingleObject(hMutex, INFINITE);
			for (list<Object*>::iterator iter = Objects.begin(); iter != Objects.end(); ++iter)
				(*iter)->involvedScene.insert(this);
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			return;
		}

		//initialize son
		if (son == 0)
		{
			son = new Scene[8];
			for (int i=0; i<8; ++i)
			{
				son[i].father = this;
				son[i].ID = i;
			}
			mid = (LFD+RBU)/2;
			son[0].LFD = LFD;
			son[0].RBU = mid;
			son[1].LFD = SimpleOBJ::Vec3f(mid.x, LFD.y, LFD.z);
			son[1].RBU = SimpleOBJ::Vec3f(RBU.x, mid.y, mid.z);
			son[2].LFD = SimpleOBJ::Vec3f(LFD.x, mid.y, LFD.z);
			son[2].RBU = SimpleOBJ::Vec3f(mid.x, RBU.y, mid.z);
			son[3].LFD = SimpleOBJ::Vec3f(mid.x, mid.y, LFD.z);
			son[3].RBU = SimpleOBJ::Vec3f(RBU.x, RBU.y, mid.z);
			son[4].LFD = SimpleOBJ::Vec3f(LFD.x, LFD.y, mid.z);
			son[4].RBU = SimpleOBJ::Vec3f(mid.x, mid.y, RBU.z);
			son[5].LFD = SimpleOBJ::Vec3f(mid.x, LFD.y, mid.z);
			son[5].RBU = SimpleOBJ::Vec3f(RBU.x, mid.y, RBU.z);
			son[6].LFD = SimpleOBJ::Vec3f(LFD.x, mid.y, mid.z);
			son[6].RBU = SimpleOBJ::Vec3f(mid.x, RBU.y, RBU.z);
			son[7].LFD = mid;
			son[7].RBU = RBU;
		}

		for (int i=0; i<8; ++i)
		{
			SimpleOBJ::Vec3f slfd = son[i].LFD, srbu = son[i].RBU;
			for (list<Object*>::iterator iter = Objects.begin(); iter != Objects.end(); ++iter)
				if ((*iter)->InScene(slfd, srbu))
				{
					son[i].Objects.push_back(*iter);
					if ((son[i].Objects.size()>>3) >= maxObjects)
						son[i].initialize();
				}
			son[i].initialize();
		}

		Objects.clear();
	}

	SimpleOBJ::Vec3f Scene::outPos(Ray& ray)
	{
		double T = FLT_MAX, t;
		int flag = -1;//0 L 1 F 2 D 3 R 4 B 5 U
		SimpleOBJ::Vec3f &Rs = ray.start, &Rd = ray.direction, outpos;
		//x=k
		if (RBU.x >= Rs.x && Rd.x > 0)
		{
			t = (RBU.x - Rs.x)/Rd.x;
			if (t < T)
			{
				T = t;
				flag = 3;
			}
		}
		else if (LFD.x <= Rs.x && Rd.x < 0)
		{
			t = (LFD.x - Rs.x)/Rd.x;
			if (t < T)
			{
				T = t;
				flag = 0;
			}
		}
		//y=k
		if (RBU.y >= Rs.y && Rd.y > 0)
		{
			t = (RBU.y - Rs.y)/Rd.y;
			if (t < T)
			{
				T = t;
				flag = 4;
			}
		}
		else if (LFD.y <= Rs.y && Rd.y < 0)
		{
			t = (LFD.y - Rs.y)/Rd.y;
			if (t < T)
			{
				T = t;
				flag = 1;
			}
		}
		//z=k
		if (RBU.z >= Rs.z && Rd.z > 0)
		{
			t = (RBU.z - Rs.z)/Rd.z;
			if (t < T)
			{
				T = t;
				flag = 5;
			}
		}
		else if (LFD.z <= Rs.z && Rd.z < 0)
		{
			t = (LFD.z - Rs.z)/Rd.z;
			if (t < T)
			{
				T = t;
				flag = 2;
			}
		}
		switch (flag)//0 L 1 F 2 D 3 R 4 B 5 U
		{
		case 0:
			outpos.x = LFD.x;
			outpos.y = Rs.y + Rd.y * T;
			outpos.z = Rs.z + Rd.z * T;
			break;
		case 1:
			outpos.y = LFD.y;
			outpos.x = Rs.x + Rd.x * T;
			outpos.z = Rs.z + Rd.z * T;
			break;
		case 2:
			outpos.z = LFD.z;
			outpos.x = Rs.x + Rd.x * T;
			outpos.y = Rs.y + Rd.y * T;
			break;
		case 3:
			outpos.x = RBU.x;
			outpos.y = Rs.y + Rd.y * T;
			outpos.z = Rs.z + Rd.z * T;
			break;
		case 4:
			outpos.y = RBU.y;
			outpos.x = Rs.x + Rd.x * T;
			outpos.z = Rs.z + Rd.z * T;
			break;
		case 5:
			outpos.z = RBU.z;
			outpos.x = Rs.x + Rd.x * T;
			outpos.y = Rs.y + Rd.y * T;
			break;
		}
// 		for (int i=0; i<3; ++i)
// 			if (outpos[i] < LFD[i])
// 				outpos[i] = LFD[i];
// 			else if (outpos[i] > RBU[i])
// 				outpos[i] = RBU[i];
		return outpos;
	}

	bool Scene::cross(Ray& ray, SimpleOBJ::Vec3f& rs_pos, vector<Object*> *objs, bool &flag, SimpleOBJ::Vec3f &pos)
	{
		objs->clear();
		flag = false;
		if (rs_pos.x >= LFD.x && rs_pos.x <= RBU.x && rs_pos.y >= LFD.y && rs_pos.y <= RBU.y && rs_pos.z >= LFD.z && rs_pos.z <= RBU.z)
		{
			pos = rs_pos;
			flag = true;
		}
		else
			pos = outPos(ray);
		int k = 0;
		for (list<Object*>::iterator iter = Objects.begin(); iter != Objects.end(); ++iter)
			if ((k = (*iter)->CrossRay(ray.start, pos)) > 0)
			{
				while (k--) objs->push_back(*iter);
				SimpleOBJ::Vec3f t = (*iter)->getTransmiss();
				if (t.r < 1e-6 && t.g < 1e-6 && t.b < 1e-6)
					break;
			}
		if (!objs->empty() || flag)
			return true;
		for (int i=0; i<3; ++i)
			if (fabs(pos[i] - __LFD[i]) < 1e-6 || fabs(pos[i] - __RBU[i]) < 1e-6)
				return true;
		return false;
	}

	//True return crossPos False return outPos
	bool Scene::cross(Ray& ray, SimpleOBJ::Vec3f& pos, Object *&obj, int &Inside)
	{
		obj = 0;
		pos = outPos(ray);
		SimpleOBJ::Vec3f crosspos;
		double T = FLT_MAX, t;
		int inside;
		for (list<Object*>::iterator iter = Objects.begin(); iter != Objects.end(); ++iter)
			if ((*iter)->CrossRay(ray.start, ray.direction, pos, crosspos, inside))
				if ((t = (crosspos-ray.start).L2Norm_Sqr()) < T)
				{
					T = t;
					obj = *iter;
					Inside = inside;
					pos = crosspos;
				}
		if (obj)
			return true;
		for (int i=0; i<3; ++i)
			if (fabs(pos[i] - __LFD[i]) < 1e-6 || fabs(pos[i] - __RBU[i]) < 1e-6)
				return true;
		return false;
	}

	void Scene::shrink()
	{
		for (int i=0; i<8; ++i)
			if (son[i].son != 0 || son[i].Objects.size() != 0)
				return;
		delete[] son;
		son = 0;
		if (father) father->shrink();
	}

// 	vector<pair<RaySource*, SimpleOBJ::Vec3f> >* Scene::countRaySources(SimpleOBJ::Vec3f &p)
// 	{
// 		SimpleOBJ::Vec3f color(0,0,0);
// 		vector<pair<RaySource*, SimpleOBJ::Vec3f> > *_raySource = new vector<pair<RaySource*, SimpleOBJ::Vec3f> >;
// 		_raySource->clear();
// 		for (vector<RaySource>::iterator iter = RaySources.begin(); iter != RaySources.end(); ++iter)
// 			_raySource->push_back(pair<RaySource*, SimpleOBJ::Vec3f>(&*iter, SimpleOBJ::Vec3f(1.0f, 1.0f, 1.0f)));
// 		for (vector<Object*>::iterator iter = AllObjects.begin(); iter != AllObjects.end(); ++iter)
// 		{
// 			for (vector<pair<RaySource*, SimpleOBJ::Vec3f> >::iterator rs = _raySource->begin(); rs != _raySource->end();)
// 				if ((*iter)->CrossRay(p, rs->first->pos))
// 				{
// 					SimpleOBJ::Vec3f transmiss = (*iter)->getTransmiss(p);
// 					if (transmiss.r < 1e-6 && transmiss.g < 1e-6 && transmiss.r < 1e-6)
// 						rs = _raySource->erase(rs);
// 					else
// 					{
// 						rs->second *= transmiss;
// 						if (rs->second.r < 1e-6 && rs->second.g < 1e-6 && rs->second.b < 1e-6)
// 							rs = _raySource->erase(rs);
// 						else
// 							++rs;
// 					}
// 				}
// 				else
// 					++rs;
// 			if (_raySource->empty()) break;
// 		}
// 		return _raySource;
// 	}
}