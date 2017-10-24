#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>

class Shader
{
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;

protected:
	ID3D11VertexShader* compileVS(const std::wstring& path, ID3DBlob** outVSBuffer = nullptr);
	ID3D11PixelShader* compilePS(const std::wstring& path, ID3DBlob** outPSBuffer = nullptr);
	ID3D11ShaderReflection* createReflection(ID3DBlob* shaderBuffer);
	ID3D11InputLayout* createInputLayout(ID3D11ShaderReflection* reflector, ID3DBlob* vsBuffer);

public:
	Shader(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~Shader();

	ID3D11Device* device() { return m_device; }
	ID3D11DeviceContext* deviceContext() { return m_deviceContext; }
};