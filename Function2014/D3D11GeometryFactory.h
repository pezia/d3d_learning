#pragma once

#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <d3d11.h>
#include <xnamath.h>
#include "IGeometryFactory.h"
#include "types.h"
#include "Mesh.h"

class D3D11GeometryFactory : public IGeometryFactory
{
private:
	ID3D11Device* dev;
	void createBuffers(Mesh * mesh);
public:
	D3D11GeometryFactory(ID3D11Device* device);
	~D3D11GeometryFactory();

	Mesh* createCube();
};
