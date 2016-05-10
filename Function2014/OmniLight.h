#pragma once

#include <DirectXMath.h>

using namespace DirectX;

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
