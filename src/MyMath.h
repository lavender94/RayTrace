#pragma once
#include <vector>
#include <map>

namespace MyMath
{
	void crossProduct(const float*, const float*, float*);
	extern inline float innerProduct(const float*, const float*);
	void normalize(float*);
	void rotate(const float *N, const float *axis, double degree, float *res);//clockwise

	//t1*x1+t2*x2=a, t2*y1+t2*y2=b
	extern inline void solve2Equation(float x1, float x2, float y1, float y2, float a, float b, float &t1, float &t2);

	void change_elem(std::vector<std::pair<float, std::pair<int, int> > >& heap, int index, std::pair<float, std::pair<int, int> >& elem, std::map<std::pair<int, int>, int>& order);
	void del_elem(std::vector<std::pair<float, std::pair<int, int> > >& heap, std::pair<int, int> tgt, std::map<std::pair<int, int>, int>& order);
}