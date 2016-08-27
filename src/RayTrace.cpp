#include "RayTrace.h"
#include "MyMath.h"
using namespace std;

namespace RayTracing
{
	SimpleOBJ::Vec3f RayTrace::BRDF(SimpleOBJ::Vec3f &pos, SimpleOBJ::Vec3f &direction, Object *obj)
	{
		SimpleOBJ::Vec3f color(0,0,0);
		for (vector<RaySource>::iterator iter = Scene::RaySources.begin(); iter != Scene::RaySources.end(); ++iter)
		{
			Ray ray(pos, iter->pos-pos);
			ray.direction.Normalize();
			SimpleOBJ::Vec3f rs_color = rsTrace(ray, subScene(pos, ray.direction, scene), *iter);
			if (rs_color.r || rs_color.g || rs_color.b)
			{
				SimpleOBJ::Vec3f l = iter->pos - pos, n = obj->NormalVector(pos);
				l.Normalize();
				SimpleOBJ::Vec3f h = (l - direction)/2;
				h.Normalize();
				float LN = MyMath::innerProduct(l._p, n._p), HN = MyMath::innerProduct(h._p, n._p);
				color += obj->getColor(pos) * obj->getDiffuse(pos) * LN * rs_color;
				if (HN >= 1e-3) color += obj->getSpecular(pos) * pow(HN, obj->getS(pos)) * rs_color;
				color += obj->getEnvironmentReflect(pos) * rs_color;
			}
		}
		return color;
	}

	bool RayTrace::Reflect(SimpleOBJ::Vec3f& pos, SimpleOBJ::Vec3f& direction, Object *obj, Ray &ray, int completeRef)
	{
		SimpleOBJ::Vec3f specular = obj->getSpecular(pos);
		if (completeRef == 2)
			specular += obj->getTransmiss(pos);
		if (specular.r < 1e-6 && specular.g < 1e-6 && specular.b < 1e-6)
			return false;
		ray.start = pos;
		SimpleOBJ::Vec3f n = obj->NormalVector(pos);
		ray.direction = direction-n*2*MyMath::innerProduct(direction._p, n._p);
		return true;
	}

	int RayTrace::Refract(SimpleOBJ::Vec3f& pos, float n0, float n1, SimpleOBJ::Vec3f& direction, Object *obj, Ray &ray)//0:no refract 1:refract 2:complete reflect
	{
		SimpleOBJ::Vec3f Transmiss = obj->getTransmiss(pos);
		if (Transmiss.r < 1e-6 && Transmiss.g < 1e-6 && Transmiss.b < 1e-6)
			return 0;
		SimpleOBJ::Vec3f n = obj->NormalVector(pos);
		float _n = n0/n1, iP = MyMath::innerProduct((-direction)._p, n._p), sint_sqr = 1-iP*iP;
		if (sint_sqr >= 1/_n/_n)
			return 2;
		ray.direction = direction*_n + n*(iP*_n - sqrt(1-_n*_n*sint_sqr));
		ray.start = pos;
		return 1;
	}

	bool RayTrace::inScene(SimpleOBJ::Vec3f &pos, SimpleOBJ::Vec3f &direction, Scene *scene)
	{
		SimpleOBJ::Vec3f LFD = scene->LFD, RBU = scene->RBU;
		for (int i=0; i<3; ++i)
			if (LFD[i]-acc_trace > pos[i] || RBU[i]+acc_trace < pos[i])
				return false;
			else if ((pos[i]-LFD[i] < acc_trace && direction[i] < 0) || (RBU[i]-pos[i] < acc_trace && direction[i] > 0))
				return false;
		return true;
	}

	Scene* RayTrace::subScene(SimpleOBJ::Vec3f &pos, SimpleOBJ::Vec3f &direction, Scene *scene)
	{
		while (scene->son)
		{
			SimpleOBJ::Vec3f &mid = scene->mid;
			int s = 0;
			if (pos.x > mid.x || (fabs(pos.x-mid.x) < acc_trace && direction.x > 0))
				s += 1;
			if (pos.y > mid.y || (fabs(pos.y-mid.y) < acc_trace && direction.y > 0))
				s += 2;
			if (pos.z > mid.z || (fabs(pos.z-mid.z) < acc_trace && direction.z > 0))
				s += 4;
			scene = scene->son+s;
		}
		return scene;
	}

	SimpleOBJ::Vec3f RayTrace::rsTrace(Ray &ray, Scene *scene, RaySource &rs)
	{
		SimpleOBJ::Vec3f rs_pos = rs.pos, color = rs.color, pos;
		vector<Object*> objs;
		bool flag = false;
		while (1)
		{
			if (scene->cross(ray, rs_pos, &objs, flag, pos))
			{
				SimpleOBJ::Vec3f Transmiss(1,1,1);
				for (vector<Object*>::iterator iter = objs.begin(); iter != objs.end(); ++iter)
				{
					Transmiss *= (*iter)->getTransmiss();
					if (Transmiss.r < acc_transmiss && Transmiss.g < acc_transmiss && Transmiss.b < acc_transmiss)
						return SimpleOBJ::Vec3f(0,0,0);
				}
				if (objs.empty() || flag)
					return color;
				else
				{
					color *= Transmiss;
					if (color.x < acc_transmiss && color.y < acc_transmiss && color.z < acc_transmiss)
						return SimpleOBJ::Vec3f(0,0,0);
					ray.start = pos;
				}
			}
			else
			{
				ray.start = pos;
				while (!inScene(pos, ray.direction, scene)) scene = scene->father;
				scene = subScene(pos, ray.direction, scene);
			}
		}
		return SimpleOBJ::Vec3f(0,0,0);
	}

	SimpleOBJ::Vec3f RayTrace::trace(Ray &ray, Scene *scene, float n0, int depth)
	{
		if (depth > max_depth) return SimpleOBJ::Vec3f(0,0,0);
		SimpleOBJ::Vec3f pos;
		Object *obj;
		int inside;
		while (1)
		{
			if (scene->cross(ray, pos, obj, inside))
				if (obj)
				{
					SimpleOBJ::Vec3f color = BRDF(pos, ray.direction, obj), temp(0,0,0);
					Ray new_ray;
					int refractFlag = 0;
					switch (inside)
					{
					case 0:
						{
							if ((refractFlag = Refract(pos, n0, n0/obj->getRefractivity(pos), ray.direction, obj, new_ray)) == 1)
								temp = trace(new_ray, scene, n0/obj->getRefractivity(pos), depth+1);
							break;
						}
					case 1:
						{
							if ((refractFlag = Refract(pos, n0, n0*obj->getRefractivity(pos), ray.direction, obj, new_ray)) == 1)
								temp = trace(new_ray, scene, n0*obj->getRefractivity(pos), depth+1);
							break;
						}
					case 2:
						{
							if ((refractFlag = Refract(pos, n0, n0*obj->getRefractivity(pos), ray.direction, obj, new_ray)) == 1)
								temp = trace(new_ray, scene, n0, depth+1);
							break;
						}
					default:
						break;
					}
					color += temp * obj->getTransmiss(pos) * obj->getColor(pos);
					if (Reflect(pos, ray.direction, obj, new_ray, refractFlag))
						temp = trace(new_ray, scene, n0, depth+1);
					if (refractFlag == 2)
						temp *= obj->getSpecular(pos)+obj->getTransmiss(pos);
					else
						temp *= obj->getSpecular(pos);
					temp *= obj->getColor(pos);
					color += temp;
					return color;
				}
				else
					return Scene::getBackground(pos, ray.direction);
			else
			{
				ray.start = pos;
				while (!inScene(pos, ray.direction, scene)) scene = scene->father;
				scene = subScene(pos, ray.direction, scene);
			}
		}
		return SimpleOBJ::Vec3f(0,0,0);
	}

	static DWORD WINAPI ThreadProc(LPVOID lpParam)
	{
		para_Threat *ray_color = (para_Threat *)lpParam;
		vector<Ray> *rays = ray_color->rays;
		vector<SimpleOBJ::Vec3f> *colors = ray_color->colors;
		RayTrace *rayTrace = ray_color->rayTrace;
		Scene *scene = rayTrace->scene;
		SimpleOBJ::Vec3f LFD = scene->LFD, RBU = scene->RBU;
		for (vector<Ray>::iterator iter = rays->begin(); iter != rays->end(); ++iter)
		{
			SimpleOBJ::Vec3f &s = iter->start;
			for (int i=0; i<3; ++i)
				if (iter->direction[i] != 0)
					if (s[i] < LFD[i])
					{
						double t = (LFD[i]-s[i])/iter->direction[i];
						s[i] = LFD[i];
						for (int j=0; j<3; ++j)
							if (j != i)
								s[j] += t*iter->direction[j];
					}
					else if (s[i] > RBU[i])
					{
						double t = (RBU[i]-s[i])/iter->direction[i];
						s[i] = RBU[i];
						for (int j=0; j<3; ++j)
							if (j != i)
								s[j] += t*iter->direction[j];
					}
			bool out = false;
			for (int i=0; i<3; ++i)
				if (s[i] < Scene::__LFD[i] || s[i] > Scene::__RBU[i])
				{
					colors->push_back(SimpleOBJ::Vec3f(0,0,0));
					out = true;
					break;
				}
			if (!out)
				colors->push_back((rayTrace->trace(*iter, rayTrace->subScene(s, iter->direction, scene), 1.0f, 0)).color_norm());
		}
		printf(".");
		return 0;
	}

	void RayTrace::Smooth(vector<SimpleOBJ::Vec3f> *&colors)
	{
		vector<SimpleOBJ::Vec3f> &color = *colors, &s_colors = *(new vector<SimpleOBJ::Vec3f>(color));
		int width = camera->width, height = camera->height-1;
		for (int i=0; i<height; ++i)
		{
			int ind_l = i*width, ind_n = ind_l+width;
			for (int j=0; j<width-1; ++j)
			{
				int temp = ind_l+j;
				s_colors[temp] += color[temp+1];
				s_colors[temp] += color[ind_n+j];
				s_colors[temp] += color[ind_n+j+1];
				s_colors[temp] /= 4;
			}
			s_colors[ind_l+width-1] += s_colors[ind_n+width-1];
			s_colors[ind_l+width-1] /= 2;
		}
		height *= width;
		for (int j=0; j<width-1; ++j)
		{
			s_colors[height+j] += s_colors[height+j+1];
			s_colors[height+j] /= 2;
		}
		delete colors;
		colors = &s_colors;
	}

	vector<SimpleOBJ::Vec3f>& RayTrace::Trace()
	{
		printf("Tracing");
		vector<SimpleOBJ::Vec3f> *color = new vector<SimpleOBJ::Vec3f>;
		if (!antiAliasing)
		{
			vector<Ray> &rays = camera->getRay();
			vector<SimpleOBJ::Vec3f> *colors = new vector<SimpleOBJ::Vec3f>[4];
			vector<Ray> *ray = new vector<Ray>[4];
			int m = camera->height*camera->width, d = m/4;
			for (int i=0; i<3; ++i)
			{
				ray[i].assign(rays.begin()+i*d, rays.begin()+(i+1)*d);
				colors[i].reserve(ray[i].size());
			}
			ray[3].assign(rays.begin()+3*d, rays.end());
			colors[3].reserve(ray[3].size());
			delete &rays;
			para_Threat *paras = new para_Threat[4];
			for (int i=0; i<4; ++i)
			{
				paras[i].rays = ray+i;
				paras[i].colors = colors+i;
				paras[i].rayTrace = this;
			}

			HANDLE hThread[4];
			hThread[0] = CreateThread(NULL, 0, ThreadProc, (LPVOID)paras, 0, 0);
			hThread[1] = CreateThread(NULL, 0, ThreadProc, (LPVOID)(paras+1), 0, 0);
			hThread[2] = CreateThread(NULL, 0, ThreadProc, (LPVOID)(paras+2), 0, 0);
			hThread[3] = CreateThread(NULL, 0, ThreadProc, (LPVOID)(paras+3), 0, 0);

			WaitForMultipleObjects(4, hThread, true, INFINITE);
			for (int i=0; i<4; ++i)
				CloseHandle(hThread[i]);

			color->reserve(m);
			for (int i=0; i<4; ++i)
				color->insert(color->end(), colors[i].begin(), colors[i].end());
			delete[] colors;
			delete[] ray;
		}
		else
		{
			vector<Ray> *rays = camera->getRay_antiAliasing();
			vector<SimpleOBJ::Vec3f> *colors = new vector<SimpleOBJ::Vec3f>[4];
			para_Threat *paras = new para_Threat[4];
			for (int i=0; i<4; ++i)
			{
				paras[i].rays = rays+i;
				paras[i].colors = colors+i;
				paras[i].rayTrace = this;
			}

			HANDLE hThread[4];
			hThread[0] = CreateThread(NULL, 0, ThreadProc, (LPVOID)paras, 0, 0);
			hThread[1] = CreateThread(NULL, 0, ThreadProc, (LPVOID)(paras+1), 0, 0);
			hThread[2] = CreateThread(NULL, 0, ThreadProc, (LPVOID)(paras+2), 0, 0);
			hThread[3] = CreateThread(NULL, 0, ThreadProc, (LPVOID)(paras+3), 0, 0);

			WaitForMultipleObjects(4, hThread, true, INFINITE);
			for (int i=0; i<4; ++i)
				CloseHandle(hThread[i]);

			
			int m = rays[0].size();
			color->reserve(m);
			for (int i=0; i<m; ++i)
				color->push_back((colors[0][i]+colors[1][i]+colors[2][i]+colors[3][i])/4);
			delete[] colors;
			delete[] rays;
		}
		if (smooth) Smooth(color);
		printf("End\n");
		return *color;
	}
}