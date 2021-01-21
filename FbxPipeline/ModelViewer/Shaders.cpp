//=================================
// Shaders.cpp
//
//=================================
#include "ModelViewer.h"
#include <comdef.h>
using namespace ModelViewer;
// このクラスの新しいインスタンスを作成します
VertexShader* VertexShader::Create(ID3D11Device* graphicsDevice,
	const void* shaderByteCode,
	SIZE_T byteCodeLength) {
	return new VertexShader(graphicsDevice, shaderByteCode, byteCodeLength);
}
// このクラスの新しいインスタンスを初期化します
VertexShader::VertexShader(ID3D11Device* graphicsDevice,
	const void* shaderByteCode,
	SIZE_T byteCodeLength) : graphicsDevice(graphicsDevice) {
	// バイトコードを内部に転写
	const auto begin = static_cast<const BYTE*>(shaderByteCode);
	this->shaderByteCode.assign(begin, begin + byteCodeLength);

	// 頂点シェーダーの生成
	const auto hr = graphicsDevice->CreateVertexShader(
		shaderByteCode, byteCodeLength, NULL, &shader);
	if (FAILED(hr)) {
		OutputDebugString(L"頂点シェーダーを作成できませんでした。");
		throw _com_error(hr);
	}
}
// デストラクター
VertexShader::~VertexShader() {
	shader->Release();
}
// このリソースを作成したグラフィックスデバイスを取得します
ID3D11Device* VertexShader::GetGraphicsDevice() {
	return graphicsDevice;
}
// ネイティブポインターを取得します
ID3D11VertexShader* VertexShader::GetNativePointer() {
	return shader;
}
// このシェーダーのバイトコードを取得します
const std::vector<BYTE>& VertexShader::GetShaderByteCode() const {
	return shaderByteCode;
}

// このクラスの新しいインスタンスを作成します
PixelShader* PixelShader::Create(ID3D11Device* graphicsDevice,
	const void* shaderByteCode,
	SIZE_T byteCodeLength) {
	return new PixelShader(graphicsDevice, shaderByteCode, byteCodeLength);
}
// このクラスの新しいインスタンスを初期化します
PixelShader::PixelShader(ID3D11Device* graphicsDevice,
	const void* shaderByteCode,
	SIZE_T byteCodeLength) {
	// シェーダーの作成
	const auto hr = graphicsDevice->CreatePixelShader(
		shaderByteCode, byteCodeLength, NULL,
		&shader);
	if (FAILED(hr)) {
		OutputDebugString(L"ピクセルシェーダーを作成できませんでした。");
		throw _com_error(hr);
	}
}
// デストラクター
PixelShader::~PixelShader() {
	shader->Release();
}
// このリソースを作成したグラフィックスデバイスを作成します
ID3D11Device* PixelShader::GetGraphicsDevice() {
	return graphicsDevice;
}
// ネイティブポインターを取得します
ID3D11PixelShader* PixelShader::GetNativePointer() {
	return shader;
}
