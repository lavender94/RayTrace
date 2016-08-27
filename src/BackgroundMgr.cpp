#include "BackgroundMgr.h"
using namespace std;

namespace RayTracing
{
	bool BackgroundMgr::next()
	{
		if (cur_background == backgoundList.size() || cur_frame < backgoundList[cur_background].frameS)
		{
			++cur_frame;
			return false;
		}
		if (cur_frame == backgoundList[cur_background].frameE-1)
		{
			Scene::Background = backgoundList[cur_background].nextBack;
			Scene::nextBackground = -1;
			Scene::alpha = 0;
		}
		else
		{
			Scene::nextBackground = backgoundList[cur_background].nextBack;
			Scene::alpha += backgoundList[cur_background].alpha;
		}
		++cur_frame;
		while (cur_background < backgoundList.size() && cur_frame >= backgoundList[cur_background].frameE) ++cur_background;
		return true;
	}
}