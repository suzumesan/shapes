#pragma once
#include <d3d11.h>
#include <memory>

class RenderDevice
{
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceCtx;
	// constant buffer maybe?

public:
	RenderDevice(ID3D11Device* device, ID3D11DeviceContext* devCtx);

	ID3D11Device* device() { return m_device; }
	ID3D11DeviceContext* deviceCtx() { return m_deviceCtx; }
};

typedef std::shared_ptr<RenderDevice> RenderDevice_SPtr;