#pragma once
#include "SimpleObject.h"

namespace RayTracing
{
	class Parser : public SimpleOBJ::CSimpleObject
	{
	public:
		int getVerticesNumber() { return m_nVertices; }
		int getTriangleNumber() { return m_nTriangles; }
		SimpleOBJ::Vec3f *getVertexList() { return m_pVertexList; }
		SimpleOBJ::Array<int,3> *getTriangleList() { return m_pTriangleList; }
	};
}