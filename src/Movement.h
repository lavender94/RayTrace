#pragma once
#include <vector>
#include <algorithm>
#include "Object.h"
#include "Vec3f.h"

namespace RayTracing
{
	class MoveInfo
	{
	public:
		MoveInfo(int type = 0) : type(type), degree(0) {}//0:stand	1:rotate	2:move	3:moveDirect	4:moveRadius

		int type;//0:stand	1:rotate	2:move	3:moveDirect	4:moveRadius	5:rotateSelf
		int frameS, frameE;//frame start end
		SimpleOBJ::Vec3f center, axis;//center(pos) axis(direction)
		union
		{
			double degree, distance;
		};

		bool operator< (const MoveInfo &v) { return frameS < v.frameS || (frameS == v.frameS && frameE < v.frameE); }
	};

	class Movement
	{
	public:
		Movement(Object *obj, bool toRefresh = true) : object(obj), cur_frame(0), cur_movement(0), refreshed(false), toRefresh(toRefresh) {}
		void init();
		void insertMove(MoveInfo &moveInfo) { movements.push_back(moveInfo); }
		bool move();

	protected:
		int cur_frame, cur_movement;
		std::vector<MoveInfo> movements;
		Object *object;	

	private:
		void repushObj();

		bool refreshed, toRefresh;

		friend class CameraMove;
	};
}