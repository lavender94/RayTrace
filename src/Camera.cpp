#include "Camera.h"
using namespace std;

namespace RayTracing
{
	vector<Ray>& Camera::getRay()
	{
		vector<Ray> *rays = new vector<Ray>;
		SimpleOBJ::Vec3f center = pos+forward*focus, tempS, tempD;
		int halfH = height/2, halfW = width/2;
		float dH = sceneHeight/height, dW = sceneWidth/width;
		for (int h = halfH; h > -halfH; --h)
			for (int w = halfW; w > -halfW; --w)
			{
				tempS = center+upward*(dH*h)+y*(dW*w);
				tempD = tempS-pos;
				tempD.Normalize();
				rays->push_back(Ray(tempS, tempD));
			}
		return *rays;
	}

	vector<Ray>* Camera::getRay_antiAliasing()
	{
		vector<Ray> *rays = new vector<Ray>[4];
		SimpleOBJ::Vec3f center = pos+forward*focus, tempS, tempD;
		int indexH = 0, indexW = 0;
		float dH = sceneHeight/height/2, dW = sceneWidth/width/2;
		for (int h = height; h > -height; --h, indexH+=2)
		{
			if (indexH == 4) indexH = 0;
			for (int w = width; w > -width; --w)
			{
				tempS = center+upward*(dH*h)+y*(dW*w);
				tempD = tempS-pos;
				tempD.Normalize();
				rays[indexH+(indexW++)].push_back(Ray(tempS, tempD));
				if (indexW == 2) indexW = 0;
			}
		}
		return rays;
	}
}