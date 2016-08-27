#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cstdlib>
#include <time.h>

#include "Parser.h"

#include "RayTrace.h"
#include "Scene.h"
#include "camera.h"

#include "Object.h"
#include "Sphere.h"
#include "Rectangle.h"
#include "Triangle.h"
#include "TriangleOBJ.h"

#include "RaySource.h"

#include "Movie.h"
using namespace std;
using namespace RayTracing;
using namespace SimpleOBJ;

#define rotateDegree 0.157f
#define Step 1

IplImage *img = 0;
char saveFileName[80] = "Image.bmp", movieInfo[80] = "Movie.inf";
bool smooth = false, antiAliasing = false, movie = false;
cv::VideoWriter vw;

void MouseHandler(int event, int x, int y, int flags, void* param)
{
	if (event == CV_EVENT_LBUTTONDOWN)
		printf("%d	%d\n", x, y);
}

void Trace(RayTrace &rayTrace)
{
	clock_t t = clock();
	vector<Vec3f> &point = rayTrace.Trace();
	int width = rayTrace.camera->width;
	if (point.size() < width*rayTrace.camera->height)
		printf("Error!\n");
	else
	{
		int i=0, j=0;
		for (vector<Vec3f>::iterator iter = point.begin(); iter != point.end(); ++iter)
		{
			cvCircle(img, cvPoint(j++, i), 0, cvScalar(iter->b, iter->g, iter->r));
			if (j == width)
			{
				j = 0;
				++i;
			}
		}
	}
	delete &point;
	printf("%f sec\n", (float)(clock()-t)/CLOCKS_PER_SEC);
	IplImage *_img = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
	cvConvertScale(img, _img, 255.0, 0);
	if (movie)
	{
		printf("Write to %s	", saveFileName);
		vw << _img;
	}
	else
	{
		printf("Save to %s	", saveFileName);
		cvSaveImage(saveFileName, _img);
	}
	printf("Success\n");
	cvReleaseImage(&_img);
	cvShowImage("MainWindow", img);
}

void load(char *filename, Scene *&scene, Camera *&camera, int &recursiveDepth)
{
	FILE *fp = fopen(filename, "r");
	printf("Load scene from:%s	", filename);
	if (!fp)
	{
		printf("Failed\n");
		exit(1);
	}
	char buf[80];
	Vec3f color, diffuse, specular, transmiss, environmentR;
	float refractivity, s;
	Parser parser;
	while (fscanf(fp, "%s", buf) != EOF)
	{
		if (buf[0] == '#')
			fscanf(fp, "%*[^\n]");
		//sphere center radius color diffuse specular transmiss environmentR refractivity s
		else if (strcmp(buf, "sphere") == 0)
		{
			Vec3f center;
			float radius;
			fscanf(fp, "%f %f %f	%f	%f %f %f	%f %f %f	%f %f %f	%f %f %f	%f %f %f	%f	%f",
				&center.x, &center.y, &center.z, &radius, 
				&color.r, &color.g, &color.b, &diffuse.r, &diffuse.g, &diffuse.b, &specular.r, &specular.g, &specular.b, &transmiss.r, &transmiss.g, &transmiss.b, &environmentR.r, &environmentR.g, &environmentR.b, &refractivity, &s);
			Scene::AllObjects.push_back(new Sphere(center, radius, color, diffuse, specular, transmiss, environmentR, refractivity, s, string("")));
		}
		//rect(rectangle) cornor length width color diffuse specular transmiss environmentR refractivity s
		else if(strcmp(buf, "rect") == 0 || strcmp(buf, "rectangle") == 0)
		{
			Vec3f cornor, length, width;
			fscanf(fp, "%f %f %f	%f %f %f	%f %f %f	%f %f %f	%f %f %f	%f %f %f	%f %f %f	%f %f %f	%f	%f",
				&cornor.x, &cornor.y, &cornor.z, &length.x, &length.y, &length.z, &width.x, &width.y, &width.z,
				&color.r, &color.g, &color.b, &diffuse.r, &diffuse.g, &diffuse.b, &specular.r, &specular.g, &specular.b, &transmiss.r, &transmiss.g, &transmiss.b, &environmentR.r, &environmentR.g, &environmentR.b, &refractivity, &s);
			Scene::AllObjects.push_back(new RayTracing::Rectangle(cornor, length, width, color, diffuse, specular, transmiss, environmentR, refractivity, s, string("")));
		}
		//triangle pos[3] color diffuse specular transmiss environmentR refractivity s
		else if (strcmp(buf, "triangle") == 0)
		{
			SimpleOBJ::Vec3f pos[3];
			fscanf(fp, "%f %f %f	%f %f %f	%f %f %f	%f %f %f	%f %f %f	%f %f %f	%f %f %f	%f %f %f	%f	%f",
				&pos[0].x, &pos[0].y, &pos[0].z, &pos[1].x, &pos[1].y, &pos[1].z, &pos[2].x, &pos[2].y, &pos[2].z,
				&color.r, &color.g, &color.b, &diffuse.r, &diffuse.g, &diffuse.b, &specular.r, &specular.g, &specular.b, &transmiss.r, &transmiss.g, &transmiss.b, &environmentR.r, &environmentR.g, &environmentR.b, &refractivity, &s);
			Scene::AllObjects.push_back(new Triangle(pos, color, diffuse, specular, transmiss, environmentR, refractivity, s, string("")));
		}
		//obj(object) path name mode center scale color diffuse specular transmiss environmentR refractivity s
		else if (strcmp(buf, "obj") == 0 || strcmp(buf, "object") == 0)
		{
			char mode[10], name[80];
			Vec3f center;
			float scale;
			fscanf(fp, "%s	%s	%s	%f %f %f	%f	%f %f %f	%f %f %f	%f %f %f	%f %f %f	%f %f %f	%f	%f",
				buf, name, mode, &center.x, &center.y, &center.z, &scale,
				&color.r, &color.g, &color.b, &diffuse.r, &diffuse.g, &diffuse.b, &specular.r, &specular.g, &specular.b, &transmiss.r, &transmiss.g, &transmiss.b, &environmentR.r, &environmentR.g, &environmentR.b, &refractivity, &s);
			printf("\n");
			parser.LoadFromObj(buf);
			if (!parser.IsLoaded())
				printf("Load obj from:%s	Failed", buf);
			else
			{
				int Fn = parser.getTriangleNumber();
				Array<int, 3> *faceList = parser.getTriangleList();
				Vec3f *vList = parser.getVertexList(), pos[3];
				if (strcmp(mode, "normal") == 0)
					for (int i=0; i<Fn; ++i)
					{
						for (int j=0; j<3; ++j)
							pos[j] = center+vList[faceList[i][j]]*scale;
						Scene::AllObjects.push_back(new Triangle(pos, color, diffuse, specular, transmiss, environmentR, refractivity, s, name));
					}
				else if (strcmp(mode, "obj") == 0 || strcmp(mode, "object") == 0)
					for (int i=0; i<Fn; ++i)
					{
						for (int j=0; j<3; ++j)
							pos[j] = center+vList[faceList[i][j]]*scale;
						Scene::AllObjects.push_back(new TriangleOBJ(pos, color, diffuse, specular, transmiss, environmentR, refractivity, s, name));
					}
				else
				{
					printf("\nUnknown mode:%s", mode);
					fscanf(fp, "%*[^\n]");
				}
				parser.Destroy();
				printf("Success");
			}
		}
		//rs(raysource) pos color [brightness]
		else if (strcmp(buf, "rs") == 0 || strcmp(buf, "raysource") == 0)
		{
			Vec3f pos;
			float brightness;
			if (fscanf(fp, "%f %f %f	%f %f %f	%f", &pos.x, &pos.y, &pos.z, &color.r, &color.g, &color.b, &brightness) < 7)
				brightness = 1.0f;
			Scene::RaySources.push_back(RaySource(pos, color, brightness));
		}
		//window
		else if (strcmp(buf, "window") == 0)
		{
			int width, height;
			fscanf(fp, "%d %d", &width, &height);
			if (img)
				cvReleaseImage(&img);
			img = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 3);
		}
		//scene
		else if (strcmp(buf, "scene") == 0)
		{
			Vec3f LFD, RBU;
			fscanf(fp, "%f %f %f	%f %f %f", &LFD.x, &LFD.y, &LFD.z, &RBU.x, &RBU.y, &RBU.z);
			scene = new Scene(LFD, RBU);
		}
		//recursive depth
		else if (strcmp(buf, "depth") == 0 || strcmp(buf, "recursivdepth") == 0)
			fscanf(fp, "%d", &recursiveDepth);
		//camera
		else if (strcmp(buf, "camera") == 0)
		{
			float width, height, focus;
			Vec3f pos, forward, upward;
			fscanf(fp, "%f %f	%f	%f %f %f	%f %f %f	%f %f %f", &width, &height, &focus, &pos.x, &pos.y, &pos.z, &forward.x, &forward.y, &forward.z, &upward.x, &upward.y, &upward.z);
			camera = new Camera(0, 0, width, height, pos, forward, upward, focus);
		}
		//texture path
		else if (strcmp(buf, "texture") == 0)
		{
			fscanf(fp, "%s", buf);
			Scene::LoadTexture(buf);
		}
		//background num
		else if (strcmp(buf, "back") == 0 || strcmp(buf, "background") == 0)
			fscanf(fp, "%d", &Scene::Background);
		//smooth 1/0
		else if (strcmp(buf, "smooth") == 0)
		{
			int s;
			fscanf(fp, "%d", &s);
			smooth = (s == 1);
		}
		//anti aliasing 1/0
		else if (strcmp(buf, "antiAliase") == 0)
		{
			int anti;
			fscanf(fp, "%d", &anti);
			antiAliasing = (anti == 1);
		}
		//movie path
		else if (strcmp(buf, "movie") == 0)
		{
			movie = true;
			fscanf(fp, "%s", movieInfo);
		}
		//save img name [path:]name
		else if (strcmp(buf, "savename") == 0 || strcmp(buf, "save") == 0)
			fscanf(fp, "%s", saveFileName);
		else
			printf("\nUnknown input: %s", buf);
	}
	fclose(fp);
	camera->width = img->width;
	camera->height = img->height;
	printf("\n");
}

int main(int argc, char **argv)
{
	cvNamedWindow("MainWindow", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("MainWindow", 100, 100);
	cvSetMouseCallback("MainWindow", MouseHandler);
	Scene *scene;
	Camera *camera;
 	int recursiveDepth = 2;
	if (argc > 1)
		load(argv[1], scene, camera, recursiveDepth);
	else
		load("scene.inf", scene, camera, recursiveDepth);
	scene->Initialize(true);
	Movie aditor(camera);
	if (movie)
	{
		aditor.Load(movieInfo);
		vw.open(saveFileName, CV_FOURCC('X', 'V', 'I', 'D'), aditor.frame_per_second, cvSize(camera->width, camera->height), true);
		if (!vw.isOpened())
		{
			printf("Cannot open %s", saveFileName);
			return 0;
		}
		aditor.reset();
	}
 	RayTrace rayTrace(camera, scene, recursiveDepth, smooth, antiAliasing);
 	Trace(rayTrace);
// 	strcpy(saveFileName, "Collection_Antilise.bmp");
// 	rayTrace.antiAliasing = true;
// 	Trace(rayTrace);
// 	strcpy(saveFileName, "Collection_Antilise_smooth.bmp");
// 	rayTrace.smooth = true;
// 	Trace(rayTrace);
	if (movie)
	{
		while (aditor.next())
		{
			printf("%d	", aditor.cur_frame);
			scene->Initialize(false);
			Trace(rayTrace);
		}
		vw.release();
	}

	bool esc = false;
	while (int k = cvWaitKey(0))
	{
		switch (k)
		{
		case 97://A
			{
				camera->move(camera->y*(-Step));
				Trace(rayTrace);
				break;
			}
		case 115://S
			{
				camera->move(camera->forward*(-Step));
				Trace(rayTrace);
				break;
			}
		case 100://D
			{
				camera->move(camera->y*Step);
				Trace(rayTrace);
				break;
			}
		case 119://W
			{
				camera->move(camera->forward*Step);
				Trace(rayTrace);
				break;
			}
		case 113://Q
			{
				camera->rotate(-rotateDegree);
				Trace(rayTrace);
				break;
			}
		case 101://E
			{
				camera->rotate(rotateDegree);
				Trace(rayTrace);
				break;
			}
		case 2490368://up
			{
				camera->upDown(rotateDegree);
				Trace(rayTrace);
				break;
			}
		case 2621440://down
			{
				camera->upDown(-rotateDegree);
				Trace(rayTrace);
				break;
			}
		case 2424832://left
			{
				camera->leftRight(rotateDegree);
				Trace(rayTrace);
				break;
			}
		case 2555904://right
			{
				camera->leftRight(-rotateDegree);
				Trace(rayTrace);
				break;
			}
		case 27://ESC
			esc = true;
			break;
		default:
			printf("%d\n", k);
		}			
		if (esc)
			break;
	}
	cvReleaseImage(&img);
	cvDestroyWindow("MainWindow");
	return 0;
}