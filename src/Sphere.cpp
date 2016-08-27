#include "Sphere.h"
#include "MyMath.h"
#include <cmath>
using namespace std;

namespace RayTracing
{
	bool Sphere::CrossRay(SimpleOBJ::Vec3f& Ls, SimpleOBJ::Vec3f& dir, SimpleOBJ::Vec3f& Le, SimpleOBJ::Vec3f& P, int &inside)//(lightStart, direction, lightEnd, crossPos)
	{
		SimpleOBJ::Vec3f l = center-Ls, _le = center-Le;
		float l_sqr = l.L2Norm_Sqr();
		if (fabs(l_sqr - r_sqr) < acc_sphere)
		{
			float _le_sqr = _le.L2Norm_Sqr();
			if (_le_sqr < r_sqr)
				return false;
			if (_le_sqr == r_sqr)
			{
				P = Le;
				inside = 0;
				return true;
			}
			float t = MyMath::innerProduct(l._p, dir._p);
			if (t < acc_sphere)
				return false;
			P = Ls + dir*(t + sqrt(r_sqr - l_sqr + t*t));
			inside = 0;
			return true;
		}
		else if (l_sqr > r_sqr)
		{
			float t = MyMath::innerProduct(l._p, dir._p);
			if (t < 0)
				return false;
			if (_le.L2Norm_Sqr() > r_sqr && MyMath::innerProduct(_le._p, dir._p) > 0)
				return false;
			float d = l_sqr - t*t;
			if (d > r_sqr)
				return false;
			float tp = sqrt(r_sqr - d);
			if (fabs(tp) < acc_sphere)
				return false;
			P = Ls + dir*(t - tp);
			inside = 1;
			return true;
		}
		else
		{
			float _le_sqr = _le.L2Norm_Sqr();
			if (_le_sqr < r_sqr)
				return false;
			if (_le_sqr == r_sqr)
			{
				P = Le;
				inside = 0;
				return true;
			}
			float t = MyMath::innerProduct(l._p, dir._p);
			P = Ls + dir*(t + sqrt(r_sqr - l_sqr + t*t));
			inside = 0;
			return true;
		}
	}

	int Sphere::CrossRay(SimpleOBJ::Vec3f& Ls, SimpleOBJ::Vec3f& Le)//do not count crossPos
	{
		SimpleOBJ::Vec3f l = center-Ls, _le = center-Le, dir = Le-Ls;
		dir.Normalize();
		float l_sqr = l.L2Norm_Sqr();
		if (fabs(l_sqr - r_sqr) < acc_sphere)
		{
			if (_le.L2Norm_Sqr() < r_sqr)
				return 0;
			if (MyMath::innerProduct(l._p, dir._p) < acc_sphere)
				return 0;
			return 1;
		}
		else if (l_sqr > r_sqr)
		{
			float t = MyMath::innerProduct(l._p, dir._p);
			if (t < 0)
				return 0;
			float _le_sqr = _le.L2Norm_Sqr();
			float tle = MyMath::innerProduct(_le._p, dir._p);
			if (_le_sqr > r_sqr && tle > 0)
				return 0;
			if (l_sqr - t*t > r_sqr)
				return 0;
			if (_le_sqr < r_sqr || (fabs(_le_sqr - r_sqr) < acc_sphere && tle > 0))
				return 1;
			return 2;
		}
		else
		{
			if (_le.L2Norm_Sqr() < r_sqr)
				return 0;
			return 1;
		}
	}

	bool Sphere::InScene(SimpleOBJ::Vec3f& LFD, SimpleOBJ::Vec3f& RBU)//(LeftFrontDown, RightBackUp)
	{
		bool flag[3] = {true, true, true};
		int count = 0;
		if (center.x >= LFD.x && center.x < RBU.x)
		{
			flag[0] = false;
			++count;
		}
		if (center.y >= LFD.y && center.y < RBU.y)
		{
			flag[1] = false;
			++count;
		}
		if (center.z >= LFD.z && center.z < RBU.z)
		{
			flag[2] = false;
			++count;
		}
		switch (count)
		{
		case 3:
			return true;
		case 2:
		{
			for (int i=0; i<3; ++i)
				if (flag[i])
					if (center[i] + radius >= LFD[i] && center[i] - radius < RBU[i])
						return true;
					else
						return false;
			return false;
		}
		case 1:
		{
			float temp = 0;
			for (int i=0; i<3; ++i)
				if (flag[i])
					if (center[i] >= LFD[i])
						temp += (center[i] - RBU[i])*(center[i]-RBU[i]);
					else
						temp += (center[i] - LFD[i])*(center[i]-LFD[i]);
			if (temp < r_sqr)
				return true;
			else
				return false;
			break;
		}
		case 0:
		{
			float temp = 0;
			for (int i=0; i<3; ++i)
				if (center[i] >= LFD[i])
					temp += (center[i] - RBU[i])*(center[i]-RBU[i]);
				else
					temp += (center[i] - LFD[i])*(center[i]-LFD[i]);
			if (temp < r_sqr)
				return true;
			else
				return false;
		}
		default:
			return false;
		}
	}

	bool Sphere::rotate(SimpleOBJ::Vec3f &center, SimpleOBJ::Vec3f &axis, double degree)
	{
		axis.Normalize();
		SimpleOBJ::Vec3f N = this->center - center, _N;
		MyMath::rotate(N._p, axis._p, degree, _N._p);
		this->center = center+_N;
		return true;
	}
}