#pragma once

#include "Mesh.h"

class IGeometryFactory
{
public:
	virtual Mesh* createCube() = 0;
};
