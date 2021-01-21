#pragma warning(disable : 26812)
#define FBXSDK_NAMESPACE fbxsdk
#include <fbxsdk.h>

// 指定したVector4の情報を表示する
void printfFbxVector4(const fbxsdk::FbxVector4& value) {
	printf("%1f, %1f, %1f, %1f\n",
		value[0], value[1], value[2], value[3]);
}
// 指定したメッシュの情報を表示する
void PrintMesh(const fbxsdk::FbxMesh* mesh) {
	printf("AtttributeType: %s (controllPointsCount: %d)\n", "eMesh",
		mesh->GetControlPointsCount());

	//頂点数を取得
	const auto controllPointsCount = mesh->GetControlPointsCount();
	// 頂点配列をすべて表示
	for (int index = 0;index < controllPointsCount; index++) {
		const auto controllPoint = mesh->GetControlPointAt(index);
		printf("controllPoint[%d] ", index);
		printfFbxVector4(controllPoint);
	}

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
void PrintNodeAttribute(fbxsdk::FbxNodeAttribute* nodeAttribute) {
	printf("%s: (Typename: %s)\n",
		nodeAttribute->GetName(), nodeAttribute->GetTypeName());

	const auto attributeType = nodeAttribute->GetAttributeType();
	switch (attributeType) {
	case FbxNodeAttribute::eMesh:
		PrintMesh(static_cast<const fbxsdk::FbxMesh*>(nodeAttribute));
	case FbxNodeAttribute::eSkeleton:
		break;
		PrintSkeleton(static_cast<const fbxsdk::FbxSkeleton*>(nodeAttribute));
		break;
	}
}

// 指定したノードの情報を表示する
void LoadNodeFromFbx(fbxsdk::FbxNode* node) {

	printf("%s :(childCount:%d, NodeAttributeCount: %d)\n",
		node->GetName(), node->GetChildCount(), node->GetNodeAttributeCount());
	//ノード内のNodeAttributeをすべて検索
	for (int index = 0; index < node->GetNodeAttributeCount(); index++) {
		const auto nodeAttribute = node->GetNodeAttributeByIndex(index);
		printf("NodeAttribute[%d]: ", index);
		PrintNodeAttribute(nodeAttribute);
	}

	for (int index = 0; index < node->GetChildCount(); index++) {
		const auto childNode = node->GetChild(index);
		printf("Node[%d]: ", index);
		LoadNodeFromFbx(childNode);
	}
}

int main() {
	//	FBX Managerを作成
	const auto manager = FbxManager::Create();
	if (manager == nullptr) {
		perror("FBX MANAGERを作成できませんでした\n");
		exit(1);
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
		exit(1);
	}

	// 入力ファイル名を指定
	if (!importer->Initialize("sample.fbx")) {
		// 指定したファイルが存在しなかったらエラーを出す
		perror("Error: FbxImporterを初期化できません。\n");
		manager->Destroy();
		exit(1);
	}

	// インポート実行
	if (!importer->Import(scene)) {
		perror("Error:シーンをインポート出来ません。\n");
		manager->Destroy();
		exit(1);
	}

	//ここでシーン内のデータを検索する
	scene->GetRootNode();

	// シーン名を取得
	printf("シーン名:%s (NodeCount: %d)\n", scene->GetName(), scene->GetNodeCount());

	// ルートノードを取得
	const auto rootnode = scene->GetRootNode();
	printf("ルートノード: ");
	LoadNodeFromFbx(rootnode);


	//FBX Managerを開放する
	manager->Destroy();

	return 0;
}
