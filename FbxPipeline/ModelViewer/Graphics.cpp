//==================================================
// Graphics.cpp
//
//==================================================
#include "ModelViewer.h"
#include <comdef.h>
using namespace ModelViewer;

// このクラスの新しいインスタンスを作成します
InputLayout* InputLayout::Create(ID3D11Device* graphicsDevice,
	const D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT numElements,
	const VertexShader* vertexShader) {
	return new InputLayout(graphicsDevice, inputElementDescs, numElements, vertexShader);
}
// このクラスの新しいインスタンスを初期化します
InputLayout::InputLayout(ID3D11Device* graphicsDevice,
	const D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT numElements,
	const VertexShader* vertexShader) {
	// 入力レイアウトを作成
	const auto hr = graphicsDevice->CreateInputLayout(
		inputElementDescs, numElements,
		vertexShader->GetShaderByteCode().data(), vertexShader->GetShaderByteCode().size(),
		&nativePointer);
	if (FAILED(hr)) {
		OutputDebugString(L"入力レイアウトを作成できませんでした。");;
		throw _com_error(hr);
	}
}
// デストラクター
InputLayout::~InputLayout() {
	nativePointer->Release();
}
// このリソースを作成したグラフィックスデバイスを取得します
ID3D11Device* InputLayout::GetGraphicsDevice() {
	return graphicsDevice;
}
// ネイティブポインターを取得します
ID3D11InputLayout* InputLayout::GetNativePointer() {
	return nativePointer;
}
