#pragma once

#include "Mesh.h"

class IGeometryFactory
{
public:
	virtual Mesh* createCube() = 0;
	virtual Mesh* createSphere(const int numSegments = 64) = 0;
	virtual Mesh* createReferenceAxis() = 0;
};
