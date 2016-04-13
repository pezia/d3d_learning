#pragma once

#include <windows.h>
#include <exception>

namespace DirectxHelper {

	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch Win32 API errors.
			throw new std::exception("Operation failed", hr);
		}
	}
}
