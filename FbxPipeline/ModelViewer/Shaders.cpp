//=================================
// Shaders.cpp
//
//=================================
#include "ModelViewer.h"
#include <comdef.h>
using namespace ModelViewer;
// ���̃N���X�̐V�����C���X�^���X���쐬���܂�
VertexShader* VertexShader::Create(ID3D11Device* graphicsDevice,
	const void* shaderByteCode,
	SIZE_T byteCodeLength) {
	return new VertexShader(graphicsDevice, shaderByteCode, byteCodeLength);
}
// ���̃N���X�̐V�����C���X�^���X�����������܂�
VertexShader::VertexShader(ID3D11Device* graphicsDevice,
	const void* shaderByteCode,
	SIZE_T byteCodeLength) : graphicsDevice(graphicsDevice) {
	// �o�C�g�R�[�h������ɓ]��
	const auto begin = static_cast<const BYTE*>(shaderByteCode);
	this->shaderByteCode.assign(begin, begin + byteCodeLength);

	// ���_�V�F�[�_�[�̐���
	const auto hr = graphicsDevice->CreateVertexShader(
		shaderByteCode, byteCodeLength, NULL, &shader);
	if (FAILED(hr)) {
		OutputDebugString(L"���_�V�F�[�_�[���쐬�ł��܂���ł����B");
		throw _com_error(hr);
	}
}
// �f�X�g���N�^�[
VertexShader::~VertexShader() {
	shader->Release();
}
// ���̃��\�[�X���쐬�����O���t�B�b�N�X�f�o�C�X���擾���܂�
ID3D11Device* VertexShader::GetGraphicsDevice() {
	return graphicsDevice;
}
// �l�C�e�B�u�|�C���^�[���擾���܂�
ID3D11VertexShader* VertexShader::GetNativePointer() {
	return shader;
}
// ���̃V�F�[�_�[�̃o�C�g�R�[�h���擾���܂�
const std::vector<BYTE>& VertexShader::GetShaderByteCode() const {
	return shaderByteCode;
}

// ���̃N���X�̐V�����C���X�^���X���쐬���܂�
PixelShader* PixelShader::Create(ID3D11Device* graphicsDevice,
	const void* shaderByteCode,
	SIZE_T byteCodeLength) {
	return new PixelShader(graphicsDevice, shaderByteCode, byteCodeLength);
}
// ���̃N���X�̐V�����C���X�^���X�����������܂�
PixelShader::PixelShader(ID3D11Device* graphicsDevice,
	const void* shaderByteCode,
	SIZE_T byteCodeLength) {
	// �V�F�[�_�[�̍쐬
	const auto hr = graphicsDevice->CreatePixelShader(
		shaderByteCode, byteCodeLength, NULL,
		&shader);
	if (FAILED(hr)) {
		OutputDebugString(L"�s�N�Z���V�F�[�_�[���쐬�ł��܂���ł����B");
		throw _com_error(hr);
	}
}
// �f�X�g���N�^�[
PixelShader::~PixelShader() {
	shader->Release();
}
// ���̃��\�[�X���쐬�����O���t�B�b�N�X�f�o�C�X���쐬���܂�
ID3D11Device* PixelShader::GetGraphicsDevice() {
	return graphicsDevice;
}
// �l�C�e�B�u�|�C���^�[���擾���܂�
ID3D11PixelShader* PixelShader::GetNativePointer() {
	return shader;
}
