#include "macros.h"
#include "Mesh.h"

Mesh::Mesh() : 
vertices(NULL), indices(NULL), 
indexCount(0), vertexCount(0),
vertexBuffer(NULL), indexBuffer(NULL)
{
}

Mesh::~Mesh()
{
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}
