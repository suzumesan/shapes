#pragma once
#include <d3d11.h>

class IRenderer
{
public:
	virtual ~IRenderer() {}
	virtual ID3D11Device* device() const = 0;
	virtual IDXGISwapChain* sc() const = 0;
	virtual ID3D11DeviceContext* dc() const = 0;
	virtual ID3D11RenderTargetView* rt() const = 0;
};
