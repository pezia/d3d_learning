#ifndef __TYPES_H__
#define __TYPES_H__

#include <windows.h>
#include <xnamath.h>

typedef struct SimpleVertex_t
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT4 Color;
} SimpleVertex;

#pragma pack(4)
typedef struct ConstantBuffer_t
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;

	XMFLOAT3 vEyePosition;

	XMFLOAT3 vLightDir[4];
	XMFLOAT4 vLightColor[4];
	FLOAT    fLightIntensity[4];

	XMFLOAT4 vAmbientColor;
	FLOAT    fAmbientIntensity;
} ConstantBuffer;

#endif