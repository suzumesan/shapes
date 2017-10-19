#pragma once
#include <d3d11.h>
#include <string>

class Shader
{
	void compileVS(const std::wstring& path);
	void compilePS(const std::wstring& path);

	ID3D11Device* m_device;
	ID3D11VertexShader* m_vs;
	ID3D11PixelShader* m_ps;
	ID3D11InputLayout* m_il;

public:
	Shader(ID3D11Device* device);
	virtual ~Shader();

	void compile(const std::wstring& path);
};