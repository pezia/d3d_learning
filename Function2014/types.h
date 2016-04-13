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


#pragma pack(push, 4)

typedef struct ConstantBuffer_t
{
	XMMATRIX mModel;
	XMMATRIX mView;
	XMMATRIX mProjection;

	XMFLOAT3 vEyePosition;

	__declspec(align(4)) INT32 iLightCount;

	__declspec(align(16)) XMFLOAT4 vLightDir[16];
	XMFLOAT4 vLightColor[16];
	FLOAT    fLightIntensity[16];

	__declspec(align(16)) XMFLOAT4 vAmbientColor;
	__declspec(align(16)) FLOAT    fAmbientIntensity;
} ConstantBuffer;

#pragma pack(pop)

#endif
