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
		ID3D11Device* fakeDevice = NULL;
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

			Assert::AreEqual(24, (int)cube->vertexCount, L"A cube has 24 vertices");
			Assert::IsNotNull(cube->vertices, L"The vertices array is initialized");
			Assert::AreEqual(36, (int)cube->indexCount, L"A cube has 36 indices");
			Assert::IsNotNull(cube->indices, L"The indices array is initialized");
			
			delete cube;
		}

	};
}