#include <fbxsdk.h>

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
	scene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::MayaYUp);
	scene->GetGlobalSettings().SetSystemUnit(FbxSystemUnit::m);

	// ルートノードを取得
	const auto rootnode = scene->GetRootNode();
	printf_s("%s\n", rootnode->GetName());

	// 子ノードを作成
	//const auto node1 = FbxNode::Create(scene, "Node1");
	//const auto node2 = FbxNode::Create(scene, "Node2");
	//const auto node3 = FbxNode::Create(scene, "Node3");
	//rootnode->AddChild(node1);
	//node1->AddChild(node3);
	//rootnode->AddChild(node2);

	// メッシュを格納するノードを作成
	const auto meshnode = FbxNode::Create(scene, "QuadNode");
	rootnode->AddChild(meshnode);

	// メッシュを作成
	const auto mesh = FbxMesh::Create(manager, "QuadMesh");
	// 3頂点分の領域を確保
	mesh->InitControlPoints(3);
	printf_s("GetControlPointsCount%d\n", mesh->GetControlPointsCount());
	// 頂点を追加
	{
		auto conrollPoint = mesh->GetControlPointAt(0);
		conrollPoint.mData[0] = 0.0f;	// x
		conrollPoint.mData[1] = 0.0f;	// y
		conrollPoint.mData[2] = 0.0f;	// z
		conrollPoint.mData[3] = 1.0f;	// w
		mesh->SetControlPointAt(conrollPoint, 0);
	}
	{
		auto conrollPoint = mesh->GetControlPointAt(0);
		conrollPoint.mData[0] = 1.0f;	// x
		conrollPoint.mData[1] = 1.0f;	// y
		conrollPoint.mData[2] = 0.0f;	// z
		conrollPoint.mData[3] = 1.0f;	// w
		mesh->SetControlPointAt(conrollPoint, 1);
	}
	{
		auto conrollPoint = mesh->GetControlPointAt(0);
		conrollPoint.mData[0] = 1.0f;	// x
		conrollPoint.mData[1] = 0.0f;	// y
		conrollPoint.mData[2] = 0.0f;	// z
		conrollPoint.mData[3] = 1.0f;	// w
		mesh->SetControlPointAt(conrollPoint, 2);
	}

	// 三角形を追加
	mesh->BeginPolygon();
	mesh->AddPolygon(0);
	mesh->AddPolygon(1);
	mesh->AddPolygon(2);
	mesh->EndPolygon();

	meshnode->SetNodeAttribute(mesh);

	// エクスポーターを作成
	const auto exporter = FbxExporter::Create(manager, "");
	if (exporter == nullptr) {
		perror("Error: FbxExporterを作成できませんでした\n");
		exit(1);
	}

	//	出力ファイル名を指定（1でテキストファイル0でバイナリーファイル）
	if (!exporter->Initialize("sample.fbx", 1)) {
		perror("Error: FbxExporterを初期化できませんでした\n");
		manager->Destroy();
		exit(1);
	}
	//	エクスポート実行
	if (!exporter->Export(scene)) {
		perror("Error:シーンをエクスポートできません\n");
		manager->Destroy();
		exit(1);
	}

	//FBX Managerを開放する
	manager->Destroy();

	return 0;
}