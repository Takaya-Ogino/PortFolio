//=============================================================================
// Game.h
// 
//=============================================================================
#pragma once
#include <vector>
#include <initguid.h>
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#pragma warning(push)
#pragma warning(disable : 26451)
#pragma warning(disable : 26495)
#pragma warning(disable : 26812)
#define FBXSDK_NAMESPACE fbxsdk
#include <fbxsdk.h>
#pragma warning(pop)

#define SAFE_RELEASE(p) if ((p) != nullptr) {(p)->Release(); (p) = nullptr;}

namespace ModelViewer {
	// ���_�o�b�t�@�[��\���܂�
	class VertexBuffer {
	public:
		// ���̃N���X�̐V�����C���X�^���X���쐬���܂�
		static VertexBuffer* Create(const fbxsdk::FbxMesh* mesh, ID3D11Device* graphicsDevice);
		// �R���X�g���N�^�[
		VertexBuffer(ID3D11Device* graphicsDevice, UINT vertexStride, UINT vertexCount);
		// �f�X�g���N�^�[
		~VertexBuffer();
		// ���_�����擾���܂�
		int GetVertexCount();
		// ���_�o�b�t�@�[�Ƀf�[�^��ݒ肵�܂�
		template <class T>
		void SetData(const std::vector<T>& data);
		// ���̃��\�[�X���쐬�����O���t�B�b�N�X�f�o�C�X���擾���܂�
		ID3D11Device* GetGraphicsDevice();
		// �l�C�e�B�u�|�C���^�[���擾���܂�
		ID3D11Buffer* GetNativePointer();

	private:
		ID3D11Device* graphicsDevice = nullptr;
		int vertexCount = 0;
		ID3D11Buffer* buffer = nullptr;

	};

	// ���_�o�b�t�@�[�Ƀf�[�^��ݒ肵�܂�
	template<class T>
	inline void ModelViewer::VertexBuffer::SetData(const std::vector<T>& data) {
		size_t elementCount = data.size();
		// �o�b�t�@�[�Ƀf�[�^��]�����܂�
		ID3D11DeviceContext* immediateContext = nullptr;
		graphicsDevice->GetImmediateContext(&immediateContext);
		immediateContext->UpdateSubresource(buffer, 0, NULL, data.data(), 0, 0);
		immediateContext->Release();
	}

	// �C���f�b�N�X�o�b�t�@�[��\���܂�
	class IndexBuffer {
	public:
		// ���̃N���X�̃C���X�^���X���쐬���܂�
		static IndexBuffer* Create(const fbxsdk::FbxMesh* mesh, ID3D11Device* graphicsDevice);
		// �R���X�g���N�^�[
		IndexBuffer(ID3D11Device* graphicsDevice, DXGI_FORMAT indexElementSize, UINT indexCount);
		// �f�X�g���N�^�[
		~IndexBuffer();
		// �C���f�b�N�X�����擾���܂�
		int GetIndexCount();
		// ���̃C���f�b�N�X�o�b�t�@�[�Ɋi�[����Ă���X�̃C���f�b�N�X�̃T�C�Y�t�H�[�}�b�g���擾���܂�
		DXGI_FORMAT GetIndexElementSize();
		// ���̃��\�[�X���쐬�����O���t�B�b�N�X�f�o�C�X���쐬���܂�
		ID3D11Device* GetGraphicsDevice();
		// �l�C�e�B�u�|�C���^�[���擾���܂�
		ID3D11Buffer* GetNativePointer();
		// �C���f�b�N�X�o�b�t�@�[�Ƀf�[�^��ݒ肵�܂�
		template<class T>
		inline void SetData(const std::vector<T>& data);

	private:
		ID3D11Device* graphicsDevice = nullptr;
		int indexCount = 0;
		DXGI_FORMAT indexElementSize = DXGI_FORMAT_R32_UINT;
		ID3D11Buffer* buffer = nullptr;
	};

	// �C���f�b�N�X�o�b�t�@�[�Ƀf�[�^��ݒ肵�܂�
	template <class T>
	inline void ModelViewer::IndexBuffer::SetData(const std::vector<T>& data) {
		size_t elementCount = data.size();
		// �o�b�t�@�[�Ƀf�[�^��]�����܂�
		ID3D11DeviceContext* immediateContext = nullptr;
		graphicsDevice->GetImmediateContext(&immediateContext);
		immediateContext->UpdateSubresource(buffer, 0, NULL, data.data(), 0, 0);
		immediateContext->Release();
	}

	// ��̒��_�Ɋ܂܂��f�[�^�̌^
	struct VertexPosition
	{
		DirectX::XMFLOAT3 position;	// �ʒu���W
	};

	// ���_�V�F�[�_�[��\���܂�
	class VertexShader {
	public:
		// ���̃N���X�̐V�����C���X�^���X���쐬���܂�
		static VertexShader* Create(ID3D11Device* graphicsDevice,
			const void* shaderByteCode, SIZE_T byteCodeLength);
		// �R���X�g���N�^�[
		VertexShader(ID3D11Device* graphicsDevice,
			const void* shaderByteCode, SIZE_T byteCodeLength);
		// �f�X�g���N�^�[
		~VertexShader();
		// ���̃��\�[�X���쐬�����O���t�B�b�N�X�f�o�C�X���擾���܂�
		ID3D11Device* GetGraphicsDevice();
		// �l�C�e�B�u�|�C���^�[���擾���܂�
		ID3D11VertexShader* GetNativePointer();
		// ���̃V�F�[�_�[�̃o�C�g�R�[�h���擾���܂�
		const std::vector<BYTE>& GetShaderByteCode() const;

	private:
		ID3D11Device* graphicsDevice = nullptr;
		ID3D11VertexShader* shader = nullptr;
		std::vector<BYTE> shaderByteCode;
	};

	// �s�N�Z���V�F�[�_�[��\���܂�
	class PixelShader {
	public:
		// ���̃N���X�̐V�����C���X�^���X���쐬���܂�
		static PixelShader* Create(ID3D11Device* graphicsDevice,
			const void* shaderByteCode,
			SIZE_T byteCodeLength);
		// ���̃N���X�̐V�����C���X�^���X�����������܂�
		PixelShader(ID3D11Device* graphicsDevice,
			const void* shaderByteCode,
			SIZE_T byteCodeLength);
		// �f�X�g���N�^�[
		~PixelShader();
		// ���̃��\�[�X���쐬�����O���t�B�b�N�X�f�o�C�X���쐬���܂�
		ID3D11Device* GetGraphicsDevice();
		// �l�C�e�B�u�|�C���^�[���擾���܂�
		ID3D11PixelShader* GetNativePointer();

	private:
		ID3D11Device* graphicsDevice = nullptr;
		ID3D11PixelShader* shader = nullptr;
	};

	// ���̓A�Z���u���[�̃��C�A�E�g��\���܂�
	class InputLayout {
	public:
		// ���̃N���X�̐V�����C���X�^���X���쐬���܂�
		static InputLayout* Create(ID3D11Device* graphicsDevice,
			const D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT numElements,
			const VertexShader* vertexShader);
		// ���̃N���X�̐V�����C���X�^���X�����������܂�
		InputLayout(ID3D11Device* graphicsDevice,
			const D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT numElements,
			const VertexShader* vertexShader);
		// �f�X�g���N�^�[
		~InputLayout();
		// ���̃��\�[�X���쐬�����O���t�B�b�N�X�f�o�C�X���擾���܂�
		ID3D11Device* GetGraphicsDevice();
		// �l�C�e�B�u�|�C���^�[���擾���܂�
		ID3D11InputLayout* GetNativePointer();

	private:
		ID3D11Device* graphicsDevice = nullptr;
		ID3D11InputLayout* nativePointer = nullptr;
	};

	// 3D�I�u�W�F�N�g�̃f�[�^��\���܂�
	class Model {
	public:
		// ���̃N���X�̐V�����C���X�^���X���쐬���܂�
		static Model* Create();
		// �f�X�g���N�^�[
		~Model();
		// ���̃��f���Ɋ܂܂��VertexBuffer���擾���܂�
		VertexBuffer* GetVertexBuffer();
		// ���̃��f����VertexBuffer��ݒ肵�܂�
		void SetVertexBuffer(VertexBuffer* value);
		// ���̃��f���Ɋ܂܂��IndexBuffer���擾���܂�
		IndexBuffer* GetIndexBuffer();
		// ���̃��f����IndexBuffer��ݒ肵�܂�
		void SetIndexBuffer(IndexBuffer* value);

	private:
		VertexBuffer* vertexBuffer = nullptr;
		IndexBuffer* indexBuffer = nullptr;
	};

	// �w�肵��Fbx�t�@�C����ǂݍ��݂܂�
	Model* LoadFbx(const std::string& fileName, ID3D11Device* graphicsDevice);
}



// �A�v���P�[�V�����S�̂�\���܂��B
class Game {
public:
	// �A�v���P�[�V���������������܂��B
	void Initialize(LPCWSTR windowTitle, int screenWidth, int screenHeight);
	// ���b�Z�[�W���[�v�����s���܂��B
	int Run();

private:
	// �E�B���h�E�̃^�C�g��
	LPCWSTR WindowTitle = L"�^�C�g��";
	// �E�B���h�E�̕�
	int ScreenWidth = 640;
	// �E�B���h�E�̍���
	int ScreenHeight = 480;
	// �E�B���h�E�̃n���h��
	HWND hWnd = NULL;

	// �E�B���h�E���쐬���܂��B
	bool InitWindow();

	// Direct3D 11�̃f�o�C�X
	ID3D11Device* graphicsDevice = nullptr;
	// Direct3D 11�̃f�o�C�X �R���e�L�X�g
	ID3D11DeviceContext* immediateContext = nullptr;
	// Direct3D 11�̋@�\���x��
	D3D_FEATURE_LEVEL featureLevel = {};
	// �X���b�v�`�F�[��
	IDXGISwapChain* swapChain = nullptr;
	// �����_�[�^�[�Q�b�g
	ID3D11RenderTargetView* renderTargetViews[1];
	// �o�b�N�o�b�t�@�[���V�F�[�_�[�ŗ��p���邽�߂̃��\�[�X �r���[
	ID3D11ShaderResourceView* renderTargetResourceView = nullptr;
	// �[�x�X�e���V���̃t�H�[�}�b�g
	const DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// �[�x�X�e���V��
	ID3D11DepthStencilView* depthStencilView = nullptr;
	// �[�x�X�e���V�����V�F�[�_�[�ŗ��p���邽�߂̃��\�[�X �r���[
	ID3D11ShaderResourceView* depthStencilResourceView = nullptr;
	// ��ʃN���A�[�Ɏg�p����J���[
	FLOAT clearColor[4] = { 53 / 255.0f, 70 / 255.0f, 166 / 255.0f, 1.0f };
	// �r���[�|�[�g
	D3D11_VIEWPORT viewports[1] = {};

	// �O���t�B�b�N�f�o�C�X���쐬���܂��B
	bool InitGraphicsDevice();
	// �O���t�B�b�N���\�[�X��������܂��B
	void ReleaseGraphicsDevice();
};
