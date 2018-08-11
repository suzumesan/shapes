#include "simple_material.h"

#include <DirectXMath.h>

using namespace DirectX;

namespace
{
	struct SimpleConstantBuffer
	{
		XMMATRIX WVP;
	};
}

SimpleMaterial::SimpleMaterial(RenderDevice_SPtr& rd) :
	Shader(rd),
	m_vs(nullptr),
	m_ps(nullptr),
	m_il(nullptr),
	m_cb(nullptr)
{
	ID3DBlob* vsBlob = nullptr;
	m_vs = compileVS(L"content/simple", &vsBlob);
	m_ps = compilePS(L"content/simple");
	ID3D11ShaderReflection* reflector = createReflection(vsBlob);
	m_il = createInputLayout(reflector, vsBlob);

	D3D11_SHADER_DESC desc;
	reflector->GetDesc(&desc);
	ID3D11ShaderReflectionConstantBuffer* reflectedConstBuffer = reflector->GetConstantBufferByIndex(0);
	D3D11_SHADER_BUFFER_DESC buffDesc;
	reflectedConstBuffer->GetDesc(&buffDesc);

	D3D11_BUFFER_DESC constBufferDesc;
	constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufferDesc.ByteWidth = buffDesc.Size;
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc.MiscFlags = 0;
	constBufferDesc.StructureByteStride = 0;

	device()->CreateBuffer(
		&constBufferDesc,
		nullptr,
		&m_cb);

	reflector->Release();
	vsBlob->Release();
}

void SimpleMaterial::begin()
{
	auto devCtx = deviceContext();
	devCtx->VSSetShader(m_vs, nullptr, 0);
	devCtx->PSSetShader(m_ps, nullptr, 0);
	devCtx->IASetInputLayout(m_il);
	devCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devCtx->VSSetConstantBuffers(0, 1, &m_cb);
}

void SimpleMaterial::render(const XMMATRIX& worldMat, const Model& model)
{
	ID3D11DeviceContext* devCtx = deviceContext();
	ID3D11Buffer* vb = model.vb();
	UINT stride = model.stride();
	UINT offset = 0;
	devCtx->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	devCtx->IASetIndexBuffer(model.ib(), DXGI_FORMAT_R32_UINT, 0);
	devCtx->DrawIndexed(model.indexCount(), 0, 0);
}

void SimpleMaterial::end()
{

}