#include "Rectangle.h"
using namespace std;

namespace RayTracing
{
	Rectangle::Rectangle(SimpleOBJ::Vec3f &corner, SimpleOBJ::Vec3f &length, SimpleOBJ::Vec3f &width, SimpleOBJ::Vec3f &color, SimpleOBJ::Vec3f& diffuse, SimpleOBJ::Vec3f& specular, SimpleOBJ::Vec3f& transmiss, SimpleOBJ::Vec3f& environmentR, float refractivity, float s, const string &name) :
		corner(corner), length(length), width(width), Object(color, diffuse, specular, transmiss, environmentR, refractivity, s, name)
	{
		MyMath::crossProduct(length._p, width._p, upward._p);
		upward.Normalize();
		countFace();
		countFlag();
	}

	bool Rectangle::CrossRay(SimpleOBJ::Vec3f& Ls, SimpleOBJ::Vec3f& dir, SimpleOBJ::Vec3f& Le, SimpleOBJ::Vec3f& P, int &inside)
	{
		if (MyMath::innerProduct(dir._p, upward._p) == 0) return false;
		float ds = MyMath::innerProduct(upward._p, Ls._p)-D, de = MyMath::innerProduct(upward._p, Le._p)-D;
		if (fabs(ds) < acc_rectangle)
			return false;
		if ((ds > 0 && de > 0) || (ds < 0 && de < 0))
			return false;
		if (fabs(de) < acc_rectangle)
			P = Le;
		else
		{
			float r = ds/(ds-de);
			P = Ls*(1-r)+Le*r;
		}
		float ratio[6] = {0}, t1, t2;//ratio:x1, x2, a, y1, y2, b
		SimpleOBJ::Vec3f _CP = P - corner;
		for (int i=0, k=0; i<3; ++i)
			if (i != flag)
			{
				ratio[k++] = length[i];
				ratio[k++] = width[i];
				ratio[k++] = _CP[i];
			}
		MyMath::solve2Equation(ratio[0], ratio[1], ratio[3], ratio[4], ratio[2], ratio[5], t1, t2);
		if (t1 > -acc_rectangle && t1-1 < acc_rectangle && t2 > -acc_rectangle && t2-1 < acc_rectangle)
		{
			inside = 2;
			return true;
		}
		return false;
	}

	int Rectangle::CrossRay(SimpleOBJ::Vec3f& Ls, SimpleOBJ::Vec3f& Le)
	{
		SimpleOBJ::Vec3f dir = Le-Ls, P;
		dir.Normalize();
		int inside;
		return CrossRay(Ls, dir, Le, P, inside) ? 1 : 0;
	}

	bool Rectangle::crossFace(SimpleOBJ::Vec3f &L1, SimpleOBJ::Vec3f &L2, SimpleOBJ::Vec3f &cor, SimpleOBJ::Vec3f &LFD, SimpleOBJ::Vec3f &RBU)
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

	bool Rectangle::InScene(SimpleOBJ::Vec3f& LFD, SimpleOBJ::Vec3f& RBU)
	{
		SimpleOBJ::Vec3f pos1 = corner+length, pos2 = corner+width, pos3 = pos1+width;
		return _inscene(LFD, RBU, corner) || _inscene(LFD, RBU, pos1) || _inscene(LFD, RBU, pos2) || _inscene(LFD, RBU, pos3)
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
			|| crossFace(corner, pos3, LFD, LFD, RBU) || crossFace(corner, pos3, RBU, LFD, RBU)
			|| crossFace(pos1, pos2, LFD, LFD, RBU) || crossFace(pos1, pos2, RBU, LFD, RBU);
	}

	bool Rectangle::rotate(SimpleOBJ::Vec3f &center, SimpleOBJ::Vec3f &axis, double degree)
	{
		SimpleOBJ::Vec3f N[3], _N[3];
		N[0] = corner-center;
		N[1] = N[0]+length;
		N[2] = N[0]+width;
		for (int i=0; i<3; ++i)
			MyMath::rotate(N[i]._p, axis._p, degree, _N[i]._p);
		corner = center + _N[0];
		length = _N[1] - _N[0];
		width = _N[2] - _N[0];
		MyMath::crossProduct(length._p, width._p, upward._p);
		upward.Normalize();
		countFace();
		countFlag();
		return true;
	}

	bool Rectangle::rotate(float degree)
	{
		if (fabs(degree) < 1e-6) return false;
		SimpleOBJ::Vec3f y, dl = length, dw = width;
		dl.Normalize();
		dw.Normalize();
		MyMath::crossProduct(upward._p, dl._p, y._p);
		upward = upward*cos(degree)-y*sin(degree);
		MyMath::crossProduct(upward._p, dl._p, y._p);
		float cosLW = MyMath::innerProduct(dl._p, dw._p);
		width = (y*sqrt(1-cosLW*cosLW)+dl*cosLW)*sqrt(width.L2Norm_Sqr());
		countFace();
		countFlag();
		return true;
	}

	bool Rectangle::upDown(float degree)
	{
		if (fabs(degree) < 1e-6) return false;
		SimpleOBJ::Vec3f dL = length, nU, nL;
		dL.Normalize();
		double sind = sin(degree), cosd = cos(degree);
		nU = upward*cosd - dL*sind;
		nL = dL*cosd - upward*sind;
		rotateTo(nL, nU);
		return true;
	}

	bool Rectangle::leftRight(float degree)
	{
		if (fabs(degree) < 1e-6) return false;
		SimpleOBJ::Vec3f dL = length, dW = width, y;
		dL.Normalize();
		dW.Normalize();
		MyMath::crossProduct(upward._p, dL._p, y._p);
		length = (dL*cos(degree) + y*sin(degree))*sqrt(length.L2Norm_Sqr());
		MyMath::crossProduct(upward._p, dL._p, y._p);
		float cosLW = MyMath::innerProduct(dL._p, dW._p);
		width = (y*sqrt(1-cosLW*cosLW)+dL*cosLW)*sqrt(width.L2Norm_Sqr());
		return true;
	}

	bool Rectangle::rotateTo(SimpleOBJ::Vec3f &forward, SimpleOBJ::Vec3f &upward)
	{
		forward.Normalize();
		upward.Normalize();
		float L = sqrt(length.L2Norm_Sqr()), W = sqrt(width.L2Norm_Sqr());
		length *= 1.0f/L;
		width *= 1.0f/W;
		float cosLW = MyMath::innerProduct(length._p, width._p);
		length = forward*L;
		this->upward = upward;
		SimpleOBJ::Vec3f y;
		MyMath::crossProduct(upward._p, forward._p, y._p);
		width = (y*sqrt(1-cosLW*cosLW)+forward*cosLW)*W;
		countFace();
		countFlag();
		return true;
	}
}