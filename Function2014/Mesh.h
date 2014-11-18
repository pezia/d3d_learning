#pragma once

#include <D3D11.h>
#include <xnamath.h>
#include "macros.h"
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
};
