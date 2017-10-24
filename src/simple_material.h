#pragma once
#include <DirectXMath.h>
#include "shader.h"
#include "model.h"

class SimpleMaterial : public Shader
{
	ID3D11VertexShader* m_vs;
	ID3D11PixelShader* m_ps;
	ID3D11InputLayout* m_il;
	ID3D11Buffer* m_cb;

public:
	SimpleMaterial(ID3D11Device* device, ID3D11DeviceContext* deviceCtx);

	void begin();
	void render(const DirectX::XMMATRIX& worldMat, const Model& model);
	void end();
};