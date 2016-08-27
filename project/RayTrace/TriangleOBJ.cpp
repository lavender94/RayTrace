#include "TriangleOBJ.h"
using namespace std;

namespace RayTracing
{
	bool TriangleOBJ::CrossRay(SimpleOBJ::Vec3f& Ls, SimpleOBJ::Vec3f& dir, SimpleOBJ::Vec3f& Le, SimpleOBJ::Vec3f& P, int &inside)
	{
		float i = MyMath::innerProduct(dir._p, upward._p);
		if (i == 0) return false;
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
			if (i > 0)
				inside = 0;
			else
				inside = 1;
			return true;
		}
		return false;
	}
}