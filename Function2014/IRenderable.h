#pragma once

#include "common.h"

class IRenderable
{
public:
	virtual void Render(DWORD tick, ID3D11DeviceContext2 *m_pDeviceContext, IDXGISwapChain1 *m_pSwapChain) = 0;
};
