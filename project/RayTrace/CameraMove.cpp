#include <cmath>
#include "CameraMove.h"
using namespace std;

namespace RayTracing
{
	void CameraMove::init()
	{
		v.init();
		f.init();
		u.init();
	}

	bool CameraMove::move()
	{
		bool movev = v.move(), movef = f.move(), moveu = u.move();
		SimpleOBJ::Vec3f forward = p_f.Center() - p_v.Center(), upward = p_u.Center() - p_v.Center();
		forward.Normalize();
		upward.Normalize();
		camera->set(p_v.Center(), forward, upward);
		return movev || moveu || movef;
	}
}