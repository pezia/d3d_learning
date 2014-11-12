#ifndef __MESH_H__
#define __MESH_H__

#include <xnamath.h>
#include "types.h"

class Mesh
{
public:
	Mesh();
	~Mesh();

	SimpleVertex *vertices;
	WORD vertexCount;

	WORD *indices;
	WORD indexCount;

	ID3D11Buffer *vertexBuffer;
	ID3D11Buffer *indexBuffer;

private:
	
};

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	if (vertices) {
		delete[] vertices;
	}

	if (indices) {
		delete[] indices;
	}

	if (vertexBuffer) {
		vertexBuffer->Release();
	}

	if (indexBuffer) {
		indexBuffer->Release();
	}
}

#endif
