//==================================================
// Graphics.cpp
//
//==================================================
#include "ModelViewer.h"
#include <comdef.h>
using namespace ModelViewer;

// ���̃N���X�̐V�����C���X�^���X���쐬���܂�
InputLayout* InputLayout::Create(ID3D11Device* graphicsDevice,
	const D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT numElements,
	const VertexShader* vertexShader) {
	return new InputLayout(graphicsDevice, inputElementDescs, numElements, vertexShader);
}
// ���̃N���X�̐V�����C���X�^���X�����������܂�
InputLayout::InputLayout(ID3D11Device* graphicsDevice,
	const D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT numElements,
	const VertexShader* vertexShader) {
	// ���̓��C�A�E�g���쐬
	const auto hr = graphicsDevice->CreateInputLayout(
		inputElementDescs, numElements,
		vertexShader->GetShaderByteCode().data(), vertexShader->GetShaderByteCode().size(),
		&nativePointer);
	if (FAILED(hr)) {
		OutputDebugString(L"���̓��C�A�E�g���쐬�ł��܂���ł����B");;
		throw _com_error(hr);
	}
}
// �f�X�g���N�^�[
InputLayout::~InputLayout() {
	nativePointer->Release();
}
// ���̃��\�[�X���쐬�����O���t�B�b�N�X�f�o�C�X���擾���܂�
ID3D11Device* InputLayout::GetGraphicsDevice() {
	return graphicsDevice;
}
// �l�C�e�B�u�|�C���^�[���擾���܂�
ID3D11InputLayout* InputLayout::GetNativePointer() {
	return nativePointer;
}
