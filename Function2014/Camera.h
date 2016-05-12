#pragma once

#include "common.h"

using namespace DirectX;

class Camera
{
public:
	void * operator new(size_t nSize)
	{
		return _aligned_malloc(nSize, 16);
	}

	void operator delete(void* p)
	{
		_aligned_free(p);
	}

	Camera() :
		fov(XM_PIDIV2),
		aspectRatio(16.0f / 9.0f),
		nearClip(0.01f),
		farClip(1000.0f)
	{
		position = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	}

	~Camera()
	{
	}

	XMMATRIX getViewMatrix()
	{
		return  XMMatrixLookAtRH(position, target, up);
	}

	XMMATRIX getProjectionMatrix()
	{
		return XMMatrixPerspectiveFovRH(fov, aspectRatio, nearClip, farClip);
	}

public:
	FLOAT fov;
	FLOAT aspectRatio;
	FLOAT nearClip;
	FLOAT farClip;
	XMVECTOR position;
	XMVECTOR target;
	XMVECTOR up;
};
