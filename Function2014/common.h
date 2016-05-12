#pragma once

#include <memory>
#include <vector>

#include <dxgi1_3.h>
#include <d3d11_2.h>

#ifdef _DEBUG
	#include <initguid.h>
	#include <dxgidebug.h>
	#include <d3d11sdklayers.h>
#endif

#include <DirectXMath.h>
#include <DirectXColors.h>

#include <wrl.h>

#include "types.h"
#include "macros.h"
#include "debug.h"
#include "DirectxHelper.h"
