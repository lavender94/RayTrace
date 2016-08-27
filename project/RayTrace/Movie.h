#pragma once
#include <vector>
#include <map>
#include "Movement.h"
#include "BackgroundMgr.h"
#include "Camera.h"
#include "CameraMove.h"

namespace RayTracing
{
	class Movie
	{
	public:
		Movie(Camera *camera, int frame = 0, int frame_per_second = 0) : cameraMove(camera), frame(frame), frame_per_second(frame_per_second), cur_frame(0) {}

		void Load(char *fileName);
		void reset();
		bool next();

		int frame, frame_per_second, cur_frame;
		std::vector<Movement*> movements;
		BackgroundMgr backgroundMgr;
		//Movement_Camera* camera_move;

	private:
		inline void addMovement(int IDs, int IDe, MoveInfo &moveinfo, std::map<int, Movement*> &map_obj_move);

		CameraMove cameraMove;
	};
}