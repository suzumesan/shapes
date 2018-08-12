#pragma once
#include <DirectXMath.h>
#include "shader.h"
#include "model.h"


class SimpleMaterial : public Shader
{
	ID3D11VertexShader* m_vs;
	ID3D11PixelShader* m_ps;
	ID3D11InputLayout* m_il;
	ID3D11Buffer* m_sharedCb;
	ID3D11Buffer* m_cb;

public:
	SimpleMaterial(RenderDevice_SPtr& rd);
	virtual ~SimpleMaterial();

	void begin(const DirectX::XMMATRIX& viewProjection);
	void render(const DirectX::XMMATRIX& worldMat, const Model& model);
	void end();
};