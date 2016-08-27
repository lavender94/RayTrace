#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "Texture.h"
using namespace std;

namespace RayTracing
{
	void Texture::Load(char *fileName)
	{
		IplImage *_texture = cvLoadImage(fileName, -1);
		if (_texture == 0)
		{
			printf("Load texture failed!\n");
			return;
		}
		IplImage *textureImg = cvCreateImage(cvGetSize(_texture), IPL_DEPTH_32F, 3);
		cvConvertScale(_texture, textureImg, 1.0/255.0, 0);
		cvReleaseImage(&_texture);
		width = textureImg->width;
		height = textureImg->height;
		texture = new SimpleOBJ::Vec3f *[height];
		CvScalar s;
		for (int i=0; i<height; ++i)
		{
			texture[i] = new SimpleOBJ::Vec3f[width];
			for (int j=0; j<width; ++j)
			{
				s = cvGet2D(textureImg, i, j);
				texture[i][j] = SimpleOBJ::Vec3f(s.val[2], s.val[1], s.val[0]);
			}
		}
		cvReleaseImage(&textureImg);
	}
}