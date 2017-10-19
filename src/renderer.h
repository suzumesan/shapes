#pragma once
#include "irenderer.h"
#include "shader.h"

class Renderer final : public IRenderer
{
	ID3D11Device* m_device;
	IDXGISwapChain* m_swapChain;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTarget;

	Shader* m_basicShader;

public:
	Renderer();
	~Renderer();

	void create(HWND handle);
	void cleanup();
	void render();

	ID3D11Device* device() const override { return m_device; }
	IDXGISwapChain* sc() const override { return m_swapChain; }
	ID3D11DeviceContext* dc() const override { return m_deviceContext; }
	ID3D11RenderTargetView* rt() const override { return m_renderTarget; }

};
