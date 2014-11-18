#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Function2014/Mesh.h"
#include "../Function2014/IGeometryFactory.h"
#include "../Function2014/D3D11GeometryFactory.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Function2014Test
{

	TEST_CLASS(GeometryFactoryTests)
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

		TEST_METHOD(TestCreateCube)
		{
			Mesh* cube = factory->createCube();

			delete cube;
		}

	};
}