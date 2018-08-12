#include "simple_material.h"

#include <DirectXMath.h>

using namespace DirectX;

namespace
{
	struct ObjectData
	{
		XMMATRIX World;
	};

	struct SharedData
	{
		XMMATRIX VP;
	};
}

SimpleMaterial::SimpleMaterial(RenderDevice_SPtr& rd) :
	Shader(rd),
	m_vs(nullptr),
	m_ps(nullptr),
	m_il(nullptr),
	m_cb(nullptr),
	m_sharedCb(nullptr)
{
	ID3DBlob* vsBlob = nullptr;
	m_vs = compileVS(L"content/simple", &vsBlob);
	m_ps = compilePS(L"content/simple");
	ID3D11ShaderReflection* reflector = createReflection(vsBlob);
	m_il = createInputLayout(reflector, vsBlob);

	ID3D11ShaderReflectionConstantBuffer* reflectedConstBuffer = reflector->GetConstantBufferByIndex(0);
	D3D11_SHADER_BUFFER_DESC buffDesc;
	reflectedConstBuffer->GetDesc(&buffDesc);
	m_sharedCb = createConstantBuffer(buffDesc.Size);

	reflectedConstBuffer = reflector->GetConstantBufferByIndex(1);
	reflectedConstBuffer->GetDesc(&buffDesc);
	m_cb = createConstantBuffer(buffDesc.Size);

	reflector->Release();
	vsBlob->Release();
}

SimpleMaterial::~SimpleMaterial()
{
	if (m_vs)
	{
		m_vs->Release();
		m_vs = nullptr;
	}

	if (m_ps)
	{
		m_ps->Release();
		m_ps = nullptr;
	}

	if (m_il)
	{
		m_il->Release();
		m_il = nullptr;
	}

	if (m_sharedCb)
	{
		m_sharedCb->Release();
		m_sharedCb = nullptr;
	}

	if (m_cb)
	{
		m_cb->Release();
		m_cb = nullptr;
	}
}

void SimpleMaterial::begin(const XMMATRIX& viewProjection)
{
	auto devCtx = deviceContext();
	devCtx->VSSetShader(m_vs, nullptr, 0);
	devCtx->PSSetShader(m_ps, nullptr, 0);
	devCtx->IASetInputLayout(m_il);
	devCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3D11Buffer* constantBuffers[2] = { m_sharedCb, m_cb };
	devCtx->VSSetConstantBuffers(0, 2, constantBuffers);

	D3D11_MAPPED_SUBRESOURCE data;
	SharedData shared;
	shared.VP = viewProjection;
	devCtx->Map(m_sharedCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy(data.pData, &shared, sizeof(shared));
	devCtx->Unmap(m_sharedCb, 0);
}

void SimpleMaterial::render(const XMMATRIX& worldMat, const Model& model)
{
	ObjectData objectData;
	objectData.World = worldMat;

	ID3D11DeviceContext* devCtx = deviceContext();
	D3D11_MAPPED_SUBRESOURCE data;
	devCtx->Map(m_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy(data.pData, &objectData, sizeof(objectData));
	devCtx->Unmap(m_cb, 0);

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