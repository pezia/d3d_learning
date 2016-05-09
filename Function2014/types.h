#ifndef __TYPES_H__
#define __TYPES_H__

#include <windows.h>
#include <xnamath.h>

typedef struct SimpleVertex_t
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
} SimpleVertex;


#pragma pack(push, 0)

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

#endif
