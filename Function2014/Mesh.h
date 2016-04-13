#pragma once

#include <string>
#include <D3D11.h>
#include <xnamath.h>
#include "macros.h"
#include "types.h"

class Mesh
{
public:
	Mesh();
	~Mesh();

	std::string name;

	D3D_PRIMITIVE_TOPOLOGY primitiveTopology;

	DWORD numVertices;
	DWORD numIndices;

	ID3D11Buffer *vertexBuffer;
	ID3D11Buffer *indexBuffer;
};
