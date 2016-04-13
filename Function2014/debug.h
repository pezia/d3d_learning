#pragma once

#pragma comment( lib, "dxguid.lib")

template<UINT TNameLength>
inline void SetDebugObjectName(_In_ ID3D11DeviceChild* resource,
	_In_z_ const char(&name)[TNameLength])
{
#if defined(_DEBUG) || defined(PROFILE)
	resource->SetPrivateData(WKPDID_D3DDebugObjectName, TNameLength - 1, name);
#endif
}
