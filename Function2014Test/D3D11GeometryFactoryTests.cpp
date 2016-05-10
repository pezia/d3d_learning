#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Function2014/Mesh.h"
#include "../Function2014/IGeometryFactory.h"
#include "../Function2014/D3D11GeometryFactory.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Function2014Test
{
	TEST_CLASS(D3D11GeometryFactoryTests)
	{
	public:
		ID3D11Device1* fakeDevice = NULL;
		IGeometryFactory* factory;

		TEST_METHOD_INITIALIZE(setUp) {
			factory = new D3D11GeometryFactory(fakeDevice);
		}
		
		TEST_METHOD_CLEANUP(tearDown) {
			delete factory;
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(CreateCube)
			TEST_DESCRIPTION(L"Test create cube method")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(CreateCube)
		{
			Mesh* cube = factory->createCube();

			Assert::AreEqual(24, (int)cube->numVertices, L"A cube has 24 vertices");
			Assert::IsNotNull(cube->vertexBuffer.Get(), L"The vertex buffer is initialized");
			Assert::AreEqual(36, (int)cube->numIndices, L"A cube has 36 indices");
			Assert::IsNotNull(cube->indexBuffer.Get(), L"The index buffer is initialized");
			
			delete cube;
		}

	};
}