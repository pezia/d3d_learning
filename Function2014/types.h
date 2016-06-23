#pragma once

#include <windows.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace DirectX;

typedef struct SimpleVertex_t
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
	XMFLOAT3 Binormal;
	XMFLOAT3 Tangent;
} SimpleVertex;


#pragma pack(push, 1)

__declspec(align(16)) typedef struct ConstantBuffer_t
{
	XMMATRIX mModel;
	XMMATRIX mView;
	XMMATRIX mProjection;

	XMVECTOR vEyePosition;

	XMVECTOR vOmniLightPosition[16];
	XMVECTOR vOmniLightColor[16];
	FLOAT fOmniLightRange[16];

	INT32 iOmniLightCount;
} ConstantBuffer;

#pragma pack(pop)
