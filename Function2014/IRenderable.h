#pragma once

#include "common.h"

class IRenderable
{
public:
	virtual void Render(DWORD tick, const D3DResources* d3dResources) = 0;
};
