#pragma once

#include <D3DX11.h>
#include <xnamath.h>

class OmniLight
{
public:
	OmniLight()
	{

	}
	~OmniLight()
	{

	}

public:
	XMFLOAT4 diffuseColor;
	XMFLOAT4 position;

	FLOAT range;
};
