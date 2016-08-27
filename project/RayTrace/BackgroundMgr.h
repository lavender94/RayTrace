#pragma once
#include <vector>
#include "Scene.h"
#include "Texture.h"

namespace RayTracing
{
	class BackgroundInfo
	{
	public:
		BackgroundInfo(int frameS, int frameE, int nextBack) : frameS(frameS), frameE(frameE), nextBack(nextBack),alpha(1.0/double(frameE-frameS)) {}

		int frameS, frameE, nextBack;
		double alpha;

		bool operator< (const BackgroundInfo &v) { return frameS < v.frameS || (frameS == v.frameS && frameE < v.frameE); }
	};

	class BackgroundMgr
	{
	public:
		BackgroundMgr() : cur_frame(0), cur_background(0) {}
		void init() { cur_frame = 0; cur_background = 0; }
		void insertBackground(BackgroundInfo &backgroundInfo) { backgoundList.push_back(backgroundInfo); }
		bool next();

	private:
		std::vector<BackgroundInfo> backgoundList;
		int cur_frame, cur_background;
	};
}