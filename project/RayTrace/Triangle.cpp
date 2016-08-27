#include "Triangle.h"
using namespace std;

namespace RayTracing
{
	Triangle::Triangle(SimpleOBJ::Vec3f *pos, SimpleOBJ::Vec3f &color, SimpleOBJ::Vec3f& diffuse, SimpleOBJ::Vec3f& specular, SimpleOBJ::Vec3f& transmiss, SimpleOBJ::Vec3f& environmentR, float refractivity, float s, const string &name) :
		Object(color, diffuse, specular, transmiss, environmentR, refractivity, s, name)
	{
		for (int i=0; i<3; ++i)
			this->pos[i] = pos[i];
		L01 = pos[1]-pos[0];
		L02 = pos[2]-pos[0];
		MyMath::crossProduct(L01._p, L02._p, upward._p);
		upward.Normalize();
		countFace();
		countFlag();
	}

	bool Triangle::CrossRay(SimpleOBJ::Vec3f& Ls, SimpleOBJ::Vec3f& dir, SimpleOBJ::Vec3f& Le, SimpleOBJ::Vec3f& P, int &inside)
	{
		if (MyMath::innerProduct(dir._p, upward._p) == 0) return false;
		float ds = MyMath::innerProduct(upward._p, Ls._p)-D, de = MyMath::innerProduct(upward._p, Le._p)-D;
		if (fabs(ds) < acc_triangle)
			return false;
		if ((ds > 0 && de > 0) || (ds < 0 && de < 0))
			return false;
		if (fabs(de) < acc_triangle)
			P = Le;
		else
		{
			float r = ds/(ds-de);
			P = Ls*(1-r)+Le*r;
		}
		float ratio[6] = {0}, t1, t2;//ratio:x1, y1, a, x2, y2, b
		SimpleOBJ::Vec3f _OP = P - pos[0];
		for (int i=0, k=0; i<3; ++i)
			if (i != flag)
			{
				ratio[k++] = L01[i];
				ratio[k++] = L02[i];
				ratio[k++] = _OP[i];
			}
		MyMath::solve2Equation(ratio[0], ratio[1], ratio[3], ratio[4], ratio[2], ratio[5], t1, t2);
		if (t1 > -acc_triangle && t2 > -acc_triangle && t1+t2 < 1+acc_triangle)
		{
			inside = 2;
			return true;
		}
		return false;
	}

	int Triangle::CrossRay(SimpleOBJ::Vec3f& Ls, SimpleOBJ::Vec3f& Le)
	{
		SimpleOBJ::Vec3f dir = Le-Ls, P;
		dir.Normalize();
		int inside;
		return CrossRay(Ls, dir, Le, P, inside) ? 1 : 0;
	}

	bool Triangle::crossFace(SimpleOBJ::Vec3f &L1, SimpleOBJ::Vec3f &L2, SimpleOBJ::Vec3f &cor, SimpleOBJ::Vec3f &LFD, SimpleOBJ::Vec3f &RBU)
	{
		//x=cor.x
		if ((L1.x <= cor.x && L2.x >= cor.x) || (L1.x > cor.x && L2.x <= cor.x))
		{
			float d1 = fabs(L1.x-cor.x), d2 = fabs(L2.x-cor.x);
			double t = d1/(d1+d2);
			SimpleOBJ::Vec3f cross = L2*t + L1*(1-t);
			if (cross.y >= LFD.y && cross.y <= RBU.y && cross.z >= LFD.z && cross.z <= RBU.z)
				return true;
		}
		//y=cor.y
		if ((L1.y <= cor.y && L2.y >= cor.y) || (L1.y > cor.y && L2.y <= cor.y))
		{
			float d1 = fabs(L1.y-cor.y), d2 = fabs(L2.y-cor.y);
			double t = d1/(d1+d2);
			SimpleOBJ::Vec3f cross = L2*t + L1*(1-t);
			if (cross.x >= LFD.x && cross.x <= RBU.x && cross.z >= LFD.z && cross.z <= RBU.z)
				return true;
		}
		//z=cor.z
		if ((L1.z <= cor.z && L2.z >= cor.z) || (L1.z > cor.z && L2.z <= cor.z))
		{
			float d1 = fabs(L1.z-cor.z), d2 = fabs(L2.z-cor.z);
			double t = d1/(d1+d2);
			SimpleOBJ::Vec3f cross = L2*t + L1*(1-t);
			if (cross.x >= LFD.x && cross.x <= RBU.x && cross.y >= LFD.y && cross.y <= RBU.y)
				return true;
		}
		return false;
	}

	bool Triangle::InScene(SimpleOBJ::Vec3f& LFD, SimpleOBJ::Vec3f& RBU)
	{
		return _inscene(LFD, RBU, pos[0]) || _inscene(LFD, RBU, pos[1]) || _inscene(LFD, RBU, pos[2])
			//z
			|| CrossRay(LFD, SimpleOBJ::Vec3f(LFD.x, LFD.y, RBU.z)) || CrossRay(SimpleOBJ::Vec3f(RBU.x, LFD.y, LFD.z), SimpleOBJ::Vec3f(RBU.x, LFD.y, RBU.z))
			|| CrossRay(SimpleOBJ::Vec3f(LFD.x, RBU.y, LFD.z), SimpleOBJ::Vec3f(LFD.x, RBU.y, RBU.z)) || CrossRay(SimpleOBJ::Vec3f(RBU.x, RBU.y, LFD.z), RBU)
			//y
			|| CrossRay(LFD, SimpleOBJ::Vec3f(LFD.x, RBU.y, LFD.z)) || CrossRay(SimpleOBJ::Vec3f(RBU.x, LFD.y, LFD.z), SimpleOBJ::Vec3f(RBU.x, RBU.y, LFD.z))
			|| CrossRay(SimpleOBJ::Vec3f(LFD.x, LFD.y, RBU.z), SimpleOBJ::Vec3f(LFD.x, RBU.y, RBU.z)) || CrossRay(SimpleOBJ::Vec3f(RBU.x, LFD.y, RBU.z), RBU)
			//x
			|| CrossRay(LFD, SimpleOBJ::Vec3f(RBU.x, LFD.y, LFD.z)) || CrossRay(SimpleOBJ::Vec3f(LFD.x, LFD.y, RBU.z), SimpleOBJ::Vec3f(RBU.x, LFD.y, RBU.z))
			|| CrossRay(SimpleOBJ::Vec3f(LFD.x, RBU.y, LFD.z), SimpleOBJ::Vec3f(RBU.x, RBU.y, LFD.z)) || CrossRay(SimpleOBJ::Vec3f(LFD.x, RBU.y, RBU.z), RBU)
			//cross cube
			|| crossFace(pos[0], pos[1], LFD, LFD, RBU) || crossFace(pos[0], pos[1], RBU, LFD, RBU)
			|| crossFace(pos[1], pos[2], LFD, LFD, RBU) || crossFace(pos[1], pos[2], RBU, LFD, RBU);
	}

	bool Triangle::rotate(SimpleOBJ::Vec3f &center, SimpleOBJ::Vec3f &axis, double degree)
	{
		SimpleOBJ::Vec3f _N, temp = Center();
		for (int i=0; i<3; ++i)
		{
			MyMath::rotate((pos[i]-center)._p, axis._p, degree, _N._p);
			pos[i] = center+_N;
		}
		temp = Center();
		L01 = pos[1]-pos[0];
		L02 = pos[2]-pos[0];
		MyMath::crossProduct(L01._p, L02._p, upward._p);
		upward.Normalize();
		countFace();
		countFlag();
		return true;
	}

	bool Triangle::rotate(float degree)
	{
		if (fabs(degree) < 1e-6) return false;
		SimpleOBJ::Vec3f y, dl = L02, dw = L01;
		dl.Normalize();
		dw.Normalize();
		MyMath::crossProduct(upward._p, dl._p, y._p);
		upward = upward*cos(degree)-y*sin(degree);
		MyMath::crossProduct(upward._p, dl._p, y._p);
		float cosLW = MyMath::innerProduct(dl._p, dw._p);
		L01 = (y*sqrt(1-cosLW*cosLW)+dl*cosLW)*sqrt(L01.L2Norm_Sqr());
		pos[1] = pos[0]+L01;
		L02 = pos[2]-pos[0];
		countFace();
		countFlag();
		return true;
	}

	bool Triangle::upDown(float degree)
	{
		if (fabs(degree) < 1e-6) return false;
		SimpleOBJ::Vec3f dL = L02, nU, nL;
		dL.Normalize();
		double sind = sin(degree), cosd = cos(degree);
		nU = upward*cosd - dL*sind;
		nL = dL*cosd - upward*sind;
		rotateTo(nL, nU);
		return true;
	}

	bool Triangle::leftRight(float degree)
	{
		if (fabs(degree) < 1e-6) return false;
		SimpleOBJ::Vec3f dL = L02, dW = L01, y;
		dL.Normalize();
		dW.Normalize();
		MyMath::crossProduct(upward._p, dL._p, y._p);
		L02 = (dL*cos(degree) + y*sin(degree))*sqrt(L02.L2Norm_Sqr());
		MyMath::crossProduct(upward._p, dL._p, y._p);
		float cosLW = MyMath::innerProduct(dL._p, dW._p);
		L01 = (y*sqrt(1-cosLW*cosLW)+dL*cosLW)*sqrt(L01.L2Norm_Sqr());
		pos[1] = pos[0]+L01;
		pos[2] = pos[0]+L02;
		return true;
	}

	bool Triangle::rotateTo(SimpleOBJ::Vec3f &forward, SimpleOBJ::Vec3f &upward)
	{
		forward.Normalize();
		upward.Normalize();
		float L = sqrt(L02.L2Norm_Sqr()), W = sqrt(L01.L2Norm_Sqr());
		L02 *= 1.0f/L;
		L01 *= 1.0f/W;
		float cosLW = MyMath::innerProduct(L02._p, L01._p);
		L02 = forward*L;
		this->upward = upward;
		SimpleOBJ::Vec3f y;
		MyMath::crossProduct(upward._p, forward._p, y._p);
		L01 = (y*sqrt(1-cosLW*cosLW)+forward*cosLW)*W;
		pos[1] = pos[0]+L01;
		pos[2] = pos[0]+L02;
		countFace();
		countFlag();
		return true;
	}
}