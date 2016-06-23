#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class OmniLight
{
public:
	OmniLight() noexcept
		: diffuseColor(), position(), range()
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
