#include <stdio.h>
#include <cstdlib>
#include <map>
#include "Movie.h"
#include "Vec3f.h"
#include "Scene.h"
using namespace std;

namespace RayTracing
{
	inline void Movie::addMovement(int IDs, int IDe, MoveInfo &moveinfo, map<int, Movement*> &map_obj_move)
	{
		for (int i=IDs; i<=IDe; ++i)
		{
			map<int, Movement*>::iterator iter = map_obj_move.find(i);
			if (iter == map_obj_move.end())
			{
				Movement *movement_obj = new Movement(Scene::AllObjects[i]);
				movement_obj->insertMove(moveinfo);
				movements.push_back(movement_obj);
				map_obj_move[i] = movement_obj;
			}
			else
				iter->second->insertMove(moveinfo);
		}
	}

	void Movie::Load(char *fileName)
	{
		FILE *fp = fopen(fileName, "r");
		if (!fp)
		{
			printf("Cannot load movie data from:%s\n", fileName);
			return;
		}
		map<int, Movement*> map_obj_move;
		char buf[80];
		int IDs, IDe;
		while (fscanf(fp, "%s", buf) != EOF)
		{
			//#comments
			if (buf[0] == '#')
			{
				fscanf(fp, "%*[^\n]");
				continue;
			}
			//movie frames frame_per_second
			else if (strcmp(buf, "movie") == 0)
			{
				fscanf(fp, "%d	%d", &frame, &frame_per_second);
				continue;
			}
			//back(background) nextBackgroundID frameS frameE
			else if (strcmp(buf, "back") == 0 || strcmp(buf, "background") == 0)
			{
				int nextBack, frameS, frameE;
				fscanf(fp, "%d	%d %d", &nextBack, &frameS, &frameE);
				backgroundMgr.insertBackground(BackgroundInfo(frameS, frameE, nextBack));
				continue;
			}
			//camera v f u
			else if (strcmp(buf, "camera") == 0)
			{
				char c[10];
				fscanf(fp, "%s %s", c, buf);
				MoveInfo moveinfo;
				if (strcmp(buf, "rotate") == 0)
				{
					moveinfo.type = 1;
					fscanf(fp, "%f %f %f	%f %f %f	%lf	%d %d",
						&moveinfo.center.x, &moveinfo.center.y, &moveinfo.center.z, &moveinfo.axis.x, &moveinfo.axis.y, &moveinfo.axis.z, 
						&moveinfo.degree, &moveinfo.frameS, &moveinfo.frameE);
				}
				//moveRadius center distance fs fe
				else if (strcmp(buf, "moveRadius") == 0)
				{
					moveinfo.type = 4;
					fscanf(fp, "%f %f %f	%lf	%d %d", 
						&moveinfo.center.x, &moveinfo.center.y, &moveinfo.center.z, &moveinfo.distance, &moveinfo.frameS, &moveinfo.frameE);
				}
				//moveDirect direction distance fs fe
				else if (strcmp(buf, "moveDirect") == 0)
				{
					moveinfo.type = 3;
					fscanf(fp, "%f %f %f	%lf	%d %d", 
						&moveinfo.axis.x, &moveinfo.axis.y, &moveinfo.axis.z, &moveinfo.distance, &moveinfo.frameS, &moveinfo.frameE);
				}
				//move pos fs fe
				else if (strcmp(buf, "move") == 0)
				{
					moveinfo.type = 2;
					fscanf(fp, "%f %f %f	%d %d", 
						&moveinfo.center.x, &moveinfo.center.y, &moveinfo.center.z, &moveinfo.frameS, &moveinfo.frameE);
				}
				if (c[0] == 'v')
					cameraMove.v.insertMove(moveinfo);
				else if (c[0] == 'f')
					cameraMove.f.insertMove(moveinfo);
				else if (c[0] == 'u')
					cameraMove.u.insertMove(moveinfo);
				else
					printf("Unknown sign:%s\n", c);
				continue;
			}
			if (sscanf(buf, "%d-%d", &IDs, &IDe) < 2)
				IDe = IDs;
			fscanf(fp, "%s", buf);
			//0:stand	1:rotate	2:move	3:moveDirect	4:moveRadius	5:rotateSelf
			//id[-id] rotate center axis degree fs fe
			if (strcmp(buf, "rotate") == 0)
			{
				MoveInfo moveinfo(1);
				fscanf(fp, "%f %f %f	%f %f %f	%lf	%d %d",
					&moveinfo.center.x, &moveinfo.center.y, &moveinfo.center.z, &moveinfo.axis.x, &moveinfo.axis.y, &moveinfo.axis.z, 
					&moveinfo.degree, &moveinfo.frameS, &moveinfo.frameE);
				addMovement(IDs, IDe, moveinfo, map_obj_move);
			}
			//id[-id] rotateSelf degree fs fe
			else if (strcmp(buf, "rotateSelf") == 0)
			{
				MoveInfo moveinfo(5);
				fscanf(fp, "%lf	%d %d", &moveinfo.degree, &moveinfo.frameS, &moveinfo.frameE);
				addMovement(IDs, IDe, moveinfo, map_obj_move);
			}
			//id[-id] moveRadius center distance fs fe
			else if (strcmp(buf, "moveRadius") == 0)
			{
				MoveInfo moveinfo(4);
				fscanf(fp, "%f %f %f	%lf	%d %d", 
					&moveinfo.center.x, &moveinfo.center.y, &moveinfo.center.z, &moveinfo.distance, &moveinfo.frameS, &moveinfo.frameE);
				addMovement(IDs, IDe, moveinfo, map_obj_move);
			}
			//id[-id] moveDirect direction distance fs fe
			else if (strcmp(buf, "moveDirect") == 0)
			{
				MoveInfo moveinfo(3);
				fscanf(fp, "%f %f %f	%lf	%d %d", 
					&moveinfo.axis.x, &moveinfo.axis.y, &moveinfo.axis.z, &moveinfo.distance, &moveinfo.frameS, &moveinfo.frameE);
				addMovement(IDs, IDe, moveinfo, map_obj_move);
			}
			//id[-id] move pos fs fe
			else if (strcmp(buf, "move") == 0)
			{
				MoveInfo moveinfo(2);
				fscanf(fp, "%f %f %f	%d %d", 
					&moveinfo.center.x, &moveinfo.center.y, &moveinfo.center.z, &moveinfo.frameS, &moveinfo.frameE);
				addMovement(IDs, IDe, moveinfo, map_obj_move);
			}
			else
				printf("Unknown format:%s\n", buf);
		}
		fclose(fp);
	}

	void Movie::reset()
	{
		for (std::vector<Movement*>::iterator iter = movements.begin(); iter != movements.end(); ++iter)
			(*iter)->init();
		backgroundMgr.init();
		cameraMove.init();
		cur_frame = 0;
	}

	bool Movie::next()
	{
		if (cur_frame == frame) return false;
		backgroundMgr.next();
		cameraMove.move();
		for (std::vector<Movement*>::iterator iter = movements.begin(); iter != movements.end(); ++iter)
			(*iter)->move();
		++cur_frame;
		return true;
	}
}