#include "shader_loader.h"

#include <d3d11.h>
#include <d3dcompiler.h>

namespace
{
	static constexpr bool DEBUG_COMPILE = false;
	static constexpr bool DEBUG_WARNING_AS_ERROR = false;

	ID3DBlob* CompileShader(
		const wchar_t* path,
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
			path,
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


void CompileVS(
	OrbD3D11Shader& shader,
	const wchar_t* path,
	OrbD3D11Wrapper& wrapper,
	const OrbD3D11ShaderLayoutDesc& desc
)
{
	static const char entryPoint[] = "main";
	static const char target[] = "vs_5_0";
	ID3DBlob* blob = CompileShader(path, entryPoint, target);
	auto hr = wrapper.Device->CreateVertexShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&shader.VS);

	assert(SUCCEEDED(hr));

	hr = wrapper.Device->CreateInputLayout(
		desc.Desc,
		desc.ElementCount,
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		&shader.InputLayout
	);

	assert(SUCCEEDED(hr));

	blob->Release();
}

void CompilePS(
	OrbD3D11Shader& shader,
	const wchar_t* path,
	OrbD3D11Wrapper& wrapper
)
{
	static const char entryPoint[] = "main";
	static const char target[] = "ps_5_0";
	ID3DBlob* blob = CompileShader(path, entryPoint, target);
	auto hr = wrapper.Device->CreatePixelShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&shader.PS);

	assert(SUCCEEDED(hr));

	blob->Release();
}

