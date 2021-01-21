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
	// 頂点バッファーを表します
	class VertexBuffer {
	public:
		// このクラスの新しいインスタンスを作成します
		static VertexBuffer* Create(const fbxsdk::FbxMesh* mesh, ID3D11Device* graphicsDevice);
		// コンストラクター
		VertexBuffer(ID3D11Device* graphicsDevice, UINT vertexStride, UINT vertexCount);
		// デストラクター
		~VertexBuffer();
		// 頂点数を取得します
		int GetVertexCount();
		// 頂点バッファーにデータを設定します
		template <class T>
		void SetData(const std::vector<T>& data);
		// このリソースを作成したグラフィックスデバイスを取得します
		ID3D11Device* GetGraphicsDevice();
		// ネイティブポインターを取得します
		ID3D11Buffer* GetNativePointer();

	private:
		ID3D11Device* graphicsDevice = nullptr;
		int vertexCount = 0;
		ID3D11Buffer* buffer = nullptr;

	};

	// 頂点バッファーにデータを設定します
	template<class T>
	inline void ModelViewer::VertexBuffer::SetData(const std::vector<T>& data) {
		size_t elementCount = data.size();
		// バッファーにデータを転送します
		ID3D11DeviceContext* immediateContext = nullptr;
		graphicsDevice->GetImmediateContext(&immediateContext);
		immediateContext->UpdateSubresource(buffer, 0, NULL, data.data(), 0, 0);
		immediateContext->Release();
	}

	// インデックスバッファーを表します
	class IndexBuffer {
	public:
		// このクラスのインスタンスを作成します
		static IndexBuffer* Create(const fbxsdk::FbxMesh* mesh, ID3D11Device* graphicsDevice);
		// コンストラクター
		IndexBuffer(ID3D11Device* graphicsDevice, DXGI_FORMAT indexElementSize, UINT indexCount);
		// デストラクター
		~IndexBuffer();
		// インデックス数を取得します
		int GetIndexCount();
		// このインデックスバッファーに格納されている個々のインデックスのサイズフォーマットを取得します
		DXGI_FORMAT GetIndexElementSize();
		// このリソースを作成したグラフィックスデバイスを作成します
		ID3D11Device* GetGraphicsDevice();
		// ネイティブポインターを取得します
		ID3D11Buffer* GetNativePointer();
		// インデックスバッファーにデータを設定します
		template<class T>
		inline void SetData(const std::vector<T>& data);

	private:
		ID3D11Device* graphicsDevice = nullptr;
		int indexCount = 0;
		DXGI_FORMAT indexElementSize = DXGI_FORMAT_R32_UINT;
		ID3D11Buffer* buffer = nullptr;
	};

	// インデックスバッファーにデータを設定します
	template <class T>
	inline void ModelViewer::IndexBuffer::SetData(const std::vector<T>& data) {
		size_t elementCount = data.size();
		// バッファーにデータを転送します
		ID3D11DeviceContext* immediateContext = nullptr;
		graphicsDevice->GetImmediateContext(&immediateContext);
		immediateContext->UpdateSubresource(buffer, 0, NULL, data.data(), 0, 0);
		immediateContext->Release();
	}

	// 一つの頂点に含まれるデータの型
	struct VertexPosition
	{
		DirectX::XMFLOAT3 position;	// 位置座標
	};

	// 頂点シェーダーを表します
	class VertexShader {
	public:
		// このクラスの新しいインスタンスを作成します
		static VertexShader* Create(ID3D11Device* graphicsDevice,
			const void* shaderByteCode, SIZE_T byteCodeLength);
		// コンストラクター
		VertexShader(ID3D11Device* graphicsDevice,
			const void* shaderByteCode, SIZE_T byteCodeLength);
		// デストラクター
		~VertexShader();
		// このリソースを作成したグラフィックスデバイスを取得します
		ID3D11Device* GetGraphicsDevice();
		// ネイティブポインターを取得します
		ID3D11VertexShader* GetNativePointer();
		// このシェーダーのバイトコードを取得します
		const std::vector<BYTE>& GetShaderByteCode() const;

	private:
		ID3D11Device* graphicsDevice = nullptr;
		ID3D11VertexShader* shader = nullptr;
		std::vector<BYTE> shaderByteCode;
	};

	// ピクセルシェーダーを表します
	class PixelShader {
	public:
		// このクラスの新しいインスタンスを作成します
		static PixelShader* Create(ID3D11Device* graphicsDevice,
			const void* shaderByteCode,
			SIZE_T byteCodeLength);
		// このクラスの新しいインスタンスを初期化します
		PixelShader(ID3D11Device* graphicsDevice,
			const void* shaderByteCode,
			SIZE_T byteCodeLength);
		// デストラクター
		~PixelShader();
		// このリソースを作成したグラフィックスデバイスを作成します
		ID3D11Device* GetGraphicsDevice();
		// ネイティブポインターを取得します
		ID3D11PixelShader* GetNativePointer();

	private:
		ID3D11Device* graphicsDevice = nullptr;
		ID3D11PixelShader* shader = nullptr;
	};

	// 入力アセンブラーのレイアウトを表します
	class InputLayout {
	public:
		// このクラスの新しいインスタンスを作成します
		static InputLayout* Create(ID3D11Device* graphicsDevice,
			const D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT numElements,
			const VertexShader* vertexShader);
		// このクラスの新しいインスタンスを初期化します
		InputLayout(ID3D11Device* graphicsDevice,
			const D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT numElements,
			const VertexShader* vertexShader);
		// デストラクター
		~InputLayout();
		// このリソースを作成したグラフィックスデバイスを取得します
		ID3D11Device* GetGraphicsDevice();
		// ネイティブポインターを取得します
		ID3D11InputLayout* GetNativePointer();

	private:
		ID3D11Device* graphicsDevice = nullptr;
		ID3D11InputLayout* nativePointer = nullptr;
	};

	// 3Dオブジェクトのデータを表します
	class Model {
	public:
		// このクラスの新しいインスタンスを作成します
		static Model* Create();
		// デストラクター
		~Model();
		// このモデルに含まれるVertexBufferを取得します
		VertexBuffer* GetVertexBuffer();
		// このモデルにVertexBufferを設定します
		void SetVertexBuffer(VertexBuffer* value);
		// このモデルに含まれるIndexBufferを取得します
		IndexBuffer* GetIndexBuffer();
		// このモデルにIndexBufferを設定します
		void SetIndexBuffer(IndexBuffer* value);

	private:
		VertexBuffer* vertexBuffer = nullptr;
		IndexBuffer* indexBuffer = nullptr;
	};

	// 指定したFbxファイルを読み込みます
	Model* LoadFbx(const std::string& fileName, ID3D11Device* graphicsDevice);
}



// アプリケーション全体を表します。
class Game {
public:
	// アプリケーションを初期化します。
	void Initialize(LPCWSTR windowTitle, int screenWidth, int screenHeight);
	// メッセージループを実行します。
	int Run();

private:
	// ウィンドウのタイトル
	LPCWSTR WindowTitle = L"タイトル";
	// ウィンドウの幅
	int ScreenWidth = 640;
	// ウィンドウの高さ
	int ScreenHeight = 480;
	// ウィンドウのハンドル
	HWND hWnd = NULL;

	// ウィンドウを作成します。
	bool InitWindow();

	// Direct3D 11のデバイス
	ID3D11Device* graphicsDevice = nullptr;
	// Direct3D 11のデバイス コンテキスト
	ID3D11DeviceContext* immediateContext = nullptr;
	// Direct3D 11の機能レベル
	D3D_FEATURE_LEVEL featureLevel = {};
	// スワップチェーン
	IDXGISwapChain* swapChain = nullptr;
	// レンダーターゲット
	ID3D11RenderTargetView* renderTargetViews[1];
	// バックバッファーをシェーダーで利用するためのリソース ビュー
	ID3D11ShaderResourceView* renderTargetResourceView = nullptr;
	// 深度ステンシルのフォーマット
	const DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 深度ステンシル
	ID3D11DepthStencilView* depthStencilView = nullptr;
	// 深度ステンシルをシェーダーで利用するためのリソース ビュー
	ID3D11ShaderResourceView* depthStencilResourceView = nullptr;
	// 画面クリアーに使用するカラー
	FLOAT clearColor[4] = { 53 / 255.0f, 70 / 255.0f, 166 / 255.0f, 1.0f };
	// ビューポート
	D3D11_VIEWPORT viewports[1] = {};

	// グラフィックデバイスを作成します。
	bool InitGraphicsDevice();
	// グラフィックリソースを解放します。
	void ReleaseGraphicsDevice();
};
