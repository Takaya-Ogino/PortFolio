#include <fbxsdk.h>

int main() {

	//	FBX Manager���쐬
	const auto manager = FbxManager::Create();
	if (manager == nullptr) {
		perror("FBX MANAGER���쐬�ł��܂���ł���\n");
		exit(1);
	}
	else {
		FBXSDK_printf("Autodesk FBX SDK version %s\n", manager->GetVersion());
	}

	//	IOSetting Object���쐬
	FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ios);


	
	//�V�[�����쐬
	const auto scene = FbxScene::Create(manager, "SceneName");
	scene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::MayaYUp);
	scene->GetGlobalSettings().SetSystemUnit(FbxSystemUnit::m);

	// ���[�g�m�[�h���擾
	const auto rootnode = scene->GetRootNode();
	printf_s("%s\n", rootnode->GetName());

	// �q�m�[�h���쐬
	//const auto node1 = FbxNode::Create(scene, "Node1");
	//const auto node2 = FbxNode::Create(scene, "Node2");
	//const auto node3 = FbxNode::Create(scene, "Node3");
	//rootnode->AddChild(node1);
	//node1->AddChild(node3);
	//rootnode->AddChild(node2);

	// ���b�V�����i�[����m�[�h���쐬
	const auto meshnode = FbxNode::Create(scene, "QuadNode");
	rootnode->AddChild(meshnode);

	// ���b�V�����쐬
	const auto mesh = FbxMesh::Create(manager, "QuadMesh");
	// 3���_���̗̈���m��
	mesh->InitControlPoints(3);
	printf_s("GetControlPointsCount%d\n", mesh->GetControlPointsCount());
	// ���_��ǉ�
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

	// �O�p�`��ǉ�
	mesh->BeginPolygon();
	mesh->AddPolygon(0);
	mesh->AddPolygon(1);
	mesh->AddPolygon(2);
	mesh->EndPolygon();

	meshnode->SetNodeAttribute(mesh);

	// �G�N�X�|�[�^�[���쐬
	const auto exporter = FbxExporter::Create(manager, "");
	if (exporter == nullptr) {
		perror("Error: FbxExporter���쐬�ł��܂���ł���\n");
		exit(1);
	}

	//	�o�̓t�@�C�������w��i1�Ńe�L�X�g�t�@�C��0�Ńo�C�i���[�t�@�C���j
	if (!exporter->Initialize("sample.fbx", 1)) {
		perror("Error: FbxExporter���������ł��܂���ł���\n");
		manager->Destroy();
		exit(1);
	}
	//	�G�N�X�|�[�g���s
	if (!exporter->Export(scene)) {
		perror("Error:�V�[�����G�N�X�|�[�g�ł��܂���\n");
		manager->Destroy();
		exit(1);
	}

	//FBX Manager���J������
	manager->Destroy();

	return 0;
}