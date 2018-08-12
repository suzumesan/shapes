#include "renderer.h"

using namespace DirectX;

namespace
{
	DXGI_SWAP_CHAIN_DESC GetSwapChainDesc(HWND handle)
	{
		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BufferCount = 1;
		desc.BufferDesc.Width = 640;
		desc.BufferDesc.Height = 480;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.OutputWindow = handle;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Windowed = TRUE;
		return desc;
	}

	void SetDefaultViewport(ID3D11DeviceContext* context)
	{
		D3D11_VIEWPORT vp;
		vp.Width = 640;
		vp.Height = 480;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		context->RSSetViewports(1, &vp);
	}

	void SetDefaultRasterSettings(ID3D11Device* dev, ID3D11DeviceContext* context)
	{
		D3D11_RASTERIZER_DESC desc;

		memset(&desc, 0, sizeof(desc));
		desc.FrontCounterClockwise = TRUE; 		// RH
		desc.CullMode = D3D11_CULL_NONE;
		desc.FillMode = D3D11_FILL_SOLID;

		ID3D11RasterizerState* state;
		dev->CreateRasterizerState(&desc, &state);
		context->RSSetState(state);
	}
}

Renderer::Renderer() :
	m_device(nullptr),
	m_swapChain(nullptr),
	m_deviceContext(nullptr),
	m_renderTarget(nullptr),
	m_rd(nullptr),
	m_material(nullptr),
	m_model(nullptr)
{

}

Renderer::~Renderer()
{
	cleanup();
}

void Renderer::create(HWND handle)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = GetSwapChainDesc(handle);

	UINT flags = 0;
#ifdef DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	const size_t levelCount = 1;
	D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL supportedLevels;
	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags,
		&level,
		levelCount,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&m_swapChain,
		&m_device,
		&supportedLevels,
		&m_deviceContext
	);

	ID3D11Texture2D* backBuffer;
	m_swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backBuffer);
	result = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTarget);
	m_deviceContext->OMSetRenderTargets(1, &m_renderTarget, nullptr);

	SetDefaultViewport(m_deviceContext);
	SetDefaultRasterSettings(m_device, m_deviceContext);

	m_rd.reset(new RenderDevice(m_device, m_deviceContext));

	m_material = new SimpleMaterial(m_rd);

	m_model = new Model(m_device);
	m_model->load("content/bun_zipper.ply");
}

void Renderer::cleanup()
{
	if (m_model)
	{
		delete m_model;
		m_model = nullptr;
	}

	if (m_material)
	{
		delete m_material;
		m_material = nullptr;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = nullptr;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = nullptr;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = nullptr;
	}

	if (m_renderTarget)
	{
		m_renderTarget->Release();
		m_renderTarget = nullptr;
	}
}

void Renderer::render()
{
	FLOAT f[4] = { 0.678f, 0.847f, 0.902f, 1 };
	m_deviceContext->ClearRenderTargetView(m_renderTarget, f);

	XMFLOAT3 eyeF3{ 0, 0, -1 };
	XMFLOAT3 lookAtF3{ 0, 0, 0 };
	XMFLOAT3 upF3{ 0, 1, 0 };
	XMMATRIX cameraView = XMMatrixLookAtLH(XMLoadFloat3(&eyeF3), XMLoadFloat3(&lookAtF3), XMLoadFloat3(&upF3));
	XMMATRIX cameraProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(40), 4.0f / 3.0f, 1.0f, 10.0f);
	XMMATRIX cameraVP = XMMatrixMultiply(cameraView, cameraProj);
	m_material->begin(cameraVP);

	XMMATRIX world = XMMatrixIdentity();
	m_material->render(world, *m_model);

	m_material->end();

	m_swapChain->Present(0, 0);
}