#pragma once

#include <D3D11.h>
#include <vector>
#include "IRenderable.h"
#include "IProperty.h"

class TimelineEventAbstract : public IRenderable
{
public:
	virtual ~TimelineEventAbstract() {};

public:
	virtual std::vector<IProperty> getProperties() = 0;
	DWORD fromTick;
	DWORD toTick;
	WORD layer;
};
