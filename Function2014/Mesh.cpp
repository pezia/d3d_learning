#include "macros.h"
#include "Mesh.h"

Mesh::Mesh() :
vertexBuffer(NULL), indexBuffer(NULL), primitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
}

Mesh::~Mesh()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}
