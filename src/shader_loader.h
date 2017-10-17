#pragma once
#include <d3d11.h>

class Shader
{
	void compileVS(const wchar_t* path);
	void compilePS(const wchar_t* path);

	ID3D11VertexShader* m_vs;
	ID3D11PixelShader* m_ps;
	ID3D11InputLayout* m_il;

public:
	Shader();
	virtual ~Shader();

	void compile(const wchar_t* path);
};