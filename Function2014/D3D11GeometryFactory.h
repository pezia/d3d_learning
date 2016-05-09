#pragma once

#include "common.h"
#include "IGeometryFactory.h"
#include "Mesh.h"

class D3D11GeometryFactory : public IGeometryFactory
{
private:
	ID3D11Device1* dev;
	void createBuffers(
		Mesh* mesh,
		SimpleVertex* vertices,
		int numVertices,
		WORD* indices,
		int numIndices
		);
public:
	D3D11GeometryFactory(ID3D11Device1* device);
	~D3D11GeometryFactory();

	Mesh* createCube();
	Mesh* createSphere(const int numSegments = 64);
	Mesh* createReferenceAxis();
};
