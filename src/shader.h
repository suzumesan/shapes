#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include "render_device.h"

class Shader
{
	RenderDevice_SPtr m_rd;

protected:
	ID3D11VertexShader* compileVS(const std::wstring& path, ID3DBlob** outVSBuffer = nullptr);
	ID3D11PixelShader* compilePS(const std::wstring& path, ID3DBlob** outPSBuffer = nullptr);
	ID3D11ShaderReflection* createReflection(ID3DBlob* shaderBuffer);
	ID3D11InputLayout* createInputLayout(ID3D11ShaderReflection* reflector, ID3DBlob* vsBuffer);

public:
	Shader(RenderDevice_SPtr& rd);
	virtual ~Shader();

	ID3D11Device* device() { return m_rd->device(); }
	ID3D11DeviceContext* deviceContext() { return m_rd->deviceCtx(); }
	RenderDevice_SPtr rd() { return m_rd; }
};