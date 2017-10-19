#include "shader.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <cassert>

namespace
{
	static constexpr bool DEBUG_COMPILE = false;
	static constexpr bool DEBUG_WARNING_AS_ERROR = false;

	ID3DBlob* compileShader(
		const std::wstring& path,
		const char* entryPoint,
		const char* target)
	{
		ID3DBlob* code;
		ID3DBlob* errorMsgs;
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
		if (DEBUG_COMPILE)
			flags |= D3DCOMPILE_DEBUG;
		if (DEBUG_WARNING_AS_ERROR)
			flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
		HRESULT hr = D3DCompileFromFile(
			path.c_str(),
			nullptr,
			nullptr,
			entryPoint,
			target,
			flags,
			0,
			&code,
			&errorMsgs
		);

		if (errorMsgs != nullptr)
		{
			OutputDebugString((wchar_t*)errorMsgs->GetBufferPointer());
			errorMsgs->Release();
			assert(FAILED(hr));
		}

		return code;
	}
}


void Shader::compileVS(const std::wstring& path)
{
	static const char entryPoint[] = "main";
	static const char target[] = "vs_5_0";
	ID3DBlob* blob = compileShader(path + L".vs", entryPoint, target);
	auto hr = m_device->CreateVertexShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&m_vs);

	assert(SUCCEEDED(hr));

	// TODO: reflection
	//hr = m_device->CreateInputLayout(
	//	desc.Desc,
	//	desc.ElementCount,
	//	blob->GetBufferPointer(),
	//	blob->GetBufferSize(),
	//	&shader.InputLayout
	//);

	assert(SUCCEEDED(hr));

	blob->Release();
}

void Shader::compilePS(const std::wstring& path)
{
	static const char entryPoint[] = "main";
	static const char target[] = "ps_5_0";
	ID3DBlob* blob = compileShader(path, entryPoint, target);
	auto hr = m_device->CreatePixelShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&m_ps);

	assert(SUCCEEDED(hr));

	blob->Release();
}

Shader::Shader(ID3D11Device* device) :
	m_device(device),
	m_vs(nullptr),
	m_ps(nullptr),
	m_il(nullptr)
{
}

Shader::~Shader()
{
	m_device = nullptr;

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
}

void Shader::compile(const std::wstring& path)
{
	compileVS(path);
	compilePS(path);
}