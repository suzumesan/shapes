#pragma once
#include <d3d11.h>

struct D3DContext
{
	ID3D11Device& device;
	IDXGISwapChain& swapChain;
	ID3D11DeviceContext& deviceContext;
	ID3D11RenderTargetView& renderTarget;
};


class Renderer
{
	ID3D11Device* m_device = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	ID3D11RenderTargetView* m_renderTarget = nullptr;

public:
	Renderer();
	~Renderer();

	void create(HWND handle);
	void cleanup();
	void render();

	ID3D11Device* device() { return m_device; }
	IDXGISwapChain* sc() { return m_swapChain; }
	ID3D11DeviceContext* dc() { return m_deviceContext; }
	ID3D11RenderTargetView* rt() { return m_renderTarget; }

};
