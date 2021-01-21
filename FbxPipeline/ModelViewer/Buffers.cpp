//=================================
// Buffers.cpp
//
//=================================
#include "ModelViewer.h"
using namespace std;
using namespace DirectX;
using namespace ModelViewer;

// 同ファイル内でのみ呼び出し可能
namespace {
	// 指定した頂点座標をFbxVector4からXMFloat3にキャストする
	XMFLOAT3 FbxVector4ToXMFloat3(const fbxsdk::FbxVector4& controllPoint) {
		XMFLOAT3 vertexPosition;
		vertexPosition.x = static_cast<float>(controllPoint[0]);
		vertexPosition.y = static_cast<float>(controllPoint[1]);
		vertexPosition.z = static_cast<float>(controllPoint[2]);
		return vertexPosition;
	}

	// 指定したメッシュの情報を表示する
	void PrintMesh(const fbxsdk::FbxMesh* mesh, Model* model, ID3D11Device* graphicsDevice) {
		// VertexBufferクラスのインスタンスを作成する
		model->SetVertexBuffer(VertexBuffer::Create(mesh, graphicsDevice));
		// IndexBufferのクラスのインスタンスを作成する
		model->SetIndexBuffer(IndexBuffer::Create(mesh, graphicsDevice));
		// ポリゴン数を取得
		const auto PolygonCount = mesh->GetPolygonCount();
		// PoligonVerticesの配列の総インデックス数を取得
		const auto polygonVertexCount = mesh->GetPolygonVertexCount();

		// 全てのポリゴンをループ処理
		for (int polygonIndex = 0; polygonIndex < PolygonCount; polygonIndex++) {
			// 一つのポリゴンを構成する頂点数を取得する
			const auto polygonSize = mesh->GetPolygonSize(polygonIndex);
			printf("PolygonSize: %d\n", polygonSize);

			// 一つのポリゴンを構成する頂点インデックスをすべて表示
			for (int positionInPolygon = 0; positionInPolygon < polygonSize; positionInPolygon++) {
				const auto polygonVertex = mesh->GetPolygonVertex(polygonIndex, positionInPolygon);
				if (positionInPolygon == 0) {
					printf("%d", polygonVertex);
				}
				else {
					printf(", %d", polygonVertex);
				}
			}
			puts("\n");
		}
	}

	// 指定したスケルトンの情報を表示する
	void PrintSkeleton(const fbxsdk::FbxSkeleton* skeleton) {
		printf("AttributeType: %s\n", "eSkeleton");
	}

	//指定したノードアトリビュートを受け取ってその情報を表示する
	void PrintNodeAttribute(fbxsdk::FbxNodeAttribute* nodeAttribute, Model* model, ID3D11Device* graphicsDevice) {
		printf("%s: (Typename: %s)\n",
			nodeAttribute->GetName(), nodeAttribute->GetTypeName());

		const auto attributeType = nodeAttribute->GetAttributeType();
		// attributeTypeに応じて分岐処理
		switch (attributeType) {
		case FbxNodeAttribute::eMesh:
			PrintMesh(static_cast<const fbxsdk::FbxMesh*>(nodeAttribute), model, graphicsDevice);
			break;
		case FbxNodeAttribute::eSkeleton:
			PrintSkeleton(static_cast<const fbxsdk::FbxSkeleton*>(nodeAttribute));
			break;
		}
	}

	// 指定したノードの情報を表示する
	void PrintNode(fbxsdk::FbxNode* node, Model* model, ID3D11Device* graphicsDevice) {

		//ノード内のNodeAttributeをすべて検索
		for (int index = 0; index < node->GetNodeAttributeCount(); index++) {
			const auto nodeAttribute = node->GetNodeAttributeByIndex(index);
			PrintNodeAttribute(nodeAttribute, model, graphicsDevice);
		}
		// 子ノードへ移行
		for (int index = 0; index < node->GetChildCount(); index++) {
			const auto childNode = node->GetChild(index);
			PrintNode(childNode, model, graphicsDevice);
		}
	}
}

// このクラスのインスタンスを作成します
VertexBuffer* VertexBuffer::Create(const fbxsdk::FbxMesh* mesh, ID3D11Device* graphicsDevice) {
	// このクラスのメモリーを確保
	VertexBuffer* vertexBuffer = new VertexBuffer(graphicsDevice, sizeof(VertexPosition), mesh->GetControlPointsCount());

	// 頂点配列を確保
	vector<VertexPosition> vertices(vertexBuffer->GetVertexCount());
	// 頂点配列をすべてFbxVector4からXMFloat3へキャスト
	for (int index = 0;index < vertexBuffer->vertexCount; index++) {
		const auto controllPoint = mesh->GetControlPointAt(index);
		vertices[index].position = FbxVector4ToXMFloat3(controllPoint);
	}
	// バッファーにデータを転送
	vertexBuffer->SetData(vertices);

	return vertexBuffer;
}
// コンストラクター
VertexBuffer::VertexBuffer(ID3D11Device* graphicsDevice, UINT vertexStride, UINT vertexCount) {
	this->graphicsDevice = graphicsDevice;
	this->vertexCount = vertexCount;

	// 作成する頂点バッファーについての記述
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = vertexStride * vertexCount;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	// 頂点バッファーを作成
	const auto hr = graphicsDevice->CreateBuffer(&bufferDesc, NULL, &buffer);
	if (FAILED(hr)) {
		OutputDebugString(L"頂点バッファーを作成できませんでした。");
	}
}
// デストラクター
VertexBuffer::~VertexBuffer() {
	buffer->Release();
}
// 頂点数を取得
int VertexBuffer::GetVertexCount() {
	return vertexCount;
}
// このリソースを作成したグラフィックスデバイスを取得します
ID3D11Device* ModelViewer::VertexBuffer::GetGraphicsDevice() {
	return graphicsDevice;
}
// ネイティブポインターを取得
ID3D11Buffer* VertexBuffer::GetNativePointer() {
	return buffer;
}

// このクラスのインスタンスを作成します
IndexBuffer* IndexBuffer::Create(const fbxsdk::FbxMesh* mesh, ID3D11Device* graphicsDevice) {
	// このクラスのメモリーを確保
	IndexBuffer* indexBuffer = new IndexBuffer(graphicsDevice, DXGI_FORMAT_R32_UINT, mesh->GetControlPointsCount());

	// インデックス配列を確保
	vector<uint32_t> indices;
	indices.reserve(indexBuffer->indexCount);
	// ポリゴン数を取得
	const auto polygonCount = mesh->GetPolygonCount();
	// 全てのポリゴンをループ処理
	for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++) {
		// 一つのポリゴンを構成する頂点数を取得する
		const auto poligonSize = mesh->GetPolygonSize(polygonIndex);
		// 一つのポリゴンを構成する頂点インデックスをすべて表示
		for (int positionInPolygon = 0; positionInPolygon < poligonSize; positionInPolygon++) {
			const auto polygonVertex = mesh->GetPolygonVertex(polygonIndex, positionInPolygon);
			indices.push_back(polygonVertex);
		}
	}
	// バッファーにデータを転送
	indexBuffer->SetData(indices);

	return indexBuffer;
}
// このクラスの新しいインスタンスを初期化します
IndexBuffer::IndexBuffer(ID3D11Device* graphicsDevice, DXGI_FORMAT indexElementSize, UINT indexCount) {
	this->graphicsDevice = graphicsDevice;
	this->indexCount = indexCount;
	this->indexElementSize = indexElementSize;

	// DXGI_FORMATから要素のサイズ（バイト数）へ変換
	UINT elementSize = 4;
	switch (indexElementSize)
	{
	case DXGI_FORMAT_R32_UINT:
		elementSize = 4;
		break;
	case DXGI_FORMAT_R16_UINT:
		elementSize = 2;
		break;
	default:
		OutputDebugString(TEXT("関数が無効な引数で呼び出されました"));
		break;
	}
	// 作成するインデックスバッファーについての記述
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = indexCount * indexElementSize;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	// インデックスバッファーを作成
	const auto hr = graphicsDevice->CreateBuffer(&bufferDesc, NULL, &buffer);
	if (FAILED(hr)) {
		OutputDebugString(L"インデックスバッファーを作成できませんでした。");
	}
}
// デストラクター
IndexBuffer::~IndexBuffer() {
	buffer->Release();
}
// インデックス数を取得します
int IndexBuffer::GetIndexCount() {
	return indexCount;
}
// このインデックスバッファーに格納されている個々のインデックスのサイズフォーマットを取得します
DXGI_FORMAT IndexBuffer::GetIndexElementSize() {
	return indexElementSize;
}
// このリソースを作成したグラフィックスデバイスを取得します
ID3D11Device* IndexBuffer::GetGraphicsDevice() {
	return graphicsDevice;
}
// ネイティブポインターを取得します
ID3D11Buffer* IndexBuffer::GetNativePointer() {
	return buffer;
}

// 指定したFbxファイルを読み込みます
Model* ModelViewer::LoadFbx(const std::string& fileName, ID3D11Device* graphicsDevice) {

	//	FBX Managerを作成
	const auto manager = FbxManager::Create();
	if (manager == nullptr) {
		perror("FBX MANAGERを作成できませんでした\n");
		return nullptr;
	}
	else {
		FBXSDK_printf("Autodesk FBX SDK version %s\n", manager->GetVersion());
	}
	//	IOSetting Objectを作成
	FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ios);
	//シーンを作成
	const auto scene = FbxScene::Create(manager, "SceneName");
	// Importerを作成
	FbxImporter* importer = FbxImporter::Create(manager, "");
	if (importer == nullptr) {
		perror("Error: FbxImporterを作成できませんでした\n");
		return nullptr;
	}
	// 入力ファイル名を指定
	if (!importer->Initialize(fileName.c_str())) {
		// 指定したファイルが存在しなかったらエラーを出す
		perror("Error: FbxImporterを初期化できません。\n");
		manager->Destroy();
		return nullptr;
	}
	// インポート実行
	if (!importer->Import(scene)) {
		perror("Error:シーンをインポート出来ません。\n");
		manager->Destroy();
		return nullptr;
	}

	const auto model = Model::Create();
	// ルートノードを取得する
	const auto rootNode = scene->GetRootNode();
	PrintNode(rootNode, model, graphicsDevice);

	HRESULT hr = S_OK;

	//開放処理
	manager->Destroy();

	return model;
}
