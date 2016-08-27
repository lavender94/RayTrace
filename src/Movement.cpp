#include <stdio.h>
#include <set>
#include "Movement.h"
#include "Scene.h"
using namespace std;

namespace RayTracing
{
	void Movement::init()
	{
		sort(movements.begin(), movements.end());
		for (vector<MoveInfo>::iterator iter = movements.begin(); iter != movements.end(); ++iter)
			switch (iter->type)//0:stand	1:rotate	2:move	3:moveDirect	4:moveRadius
			{
			case 1:
				iter->degree /= iter->frameE-iter->frameS;
				iter->axis.Normalize();
				break;
			case 3:
				iter->distance /= iter->frameE-iter->frameS;
				iter->axis.Normalize();
				iter->axis *= iter->distance;
				break;
			case 4:
				iter->distance /= iter->frameE-iter->frameS;
				break;
			case 5:
				iter->degree /= iter->frameE-iter->frameS;
				break;
			}
		cur_frame = 0;
		cur_movement = 0;
	}

	bool Movement::move()
	{
		refreshed = false;
		if (cur_movement == movements.size() || cur_frame < movements[cur_movement].frameS)
		{
			++cur_frame;
			return false;
		}
		MoveInfo *cur_move;
		int _cur_pos = cur_movement;
		while ((cur_move = &movements[_cur_pos++])->frameS <= cur_frame)
		{
			switch (cur_move->type)//0:stand	1:rotate	2:move	3:moveDirect	4:moveRadius	5:rotateSelf
			{
			case 1:
				{
					if (object->rotate(cur_move->center, cur_move->axis, cur_move->degree))
						repushObj();
					else
						printf("Cannot rotate object %s", object->getName().c_str());
					break;
				}
			case 2:
				{
					if (cur_frame == cur_move->frameS)
						cur_move->axis = (cur_move->center-object->Center()) / (cur_move->frameE-cur_move->frameS);
					if (cur_frame == cur_move->frameE-1)
						if (object->moveTo(cur_move->center))
							repushObj();
						else
							printf("Cannot move object %s", object->getName().c_str());
					else
						if (object->move((cur_move->center-object->Center())/(cur_move->frameE-cur_frame)))
							repushObj();
						else
							printf("Cannot move object %s", object->getName().c_str());
					break;
				}
			case 3:
				{
					if (object->move(cur_move->axis))
						repushObj();
					else
						printf("Cannot move object %s", object->getName().c_str());
					break;
				}
			case 4:
				{
					SimpleOBJ::Vec3f direction = object->Center()-cur_move->center;
					direction.Normalize();
					if (object->move(direction*cur_move->distance))
						repushObj();
					else
						printf("Cannot move object %s", object->getName().c_str());
					break;
				}
			case 5:
				{
					if (object->rotate(object->Center(), object->NormalVector(object->Center()), cur_move->degree))
						repushObj();
					else
						printf("Cannot rotate object %s", object->getName().c_str());
					break;
				}
			}
			if (_cur_pos == movements.size())
				break;
		}
		++cur_frame;
		while (cur_movement < movements.size() && cur_frame >= movements[cur_movement].frameE) ++cur_movement;
		return true;
	}

	void Movement::repushObj()
	{
		if (!toRefresh || refreshed) return;
		refreshed = true;
		set<Scene*> &involve = object->involvedScene;
		for (set<Scene*>::iterator iter = involve.begin(); iter != involve.end(); ++iter)
			for (list<Object*>::iterator it = (*iter)->Objects.begin(); it != (*iter)->Objects.end(); ++it)
				if ((*it) == object)
				{
					(*iter)->Objects.erase(it);
					if ((*iter)->Objects.empty() && (*iter)->father)
						(*iter)->father->shrink();
					break;
				}
		involve.clear();
		Scene::WaitObjects.push_back(object);
	}
}