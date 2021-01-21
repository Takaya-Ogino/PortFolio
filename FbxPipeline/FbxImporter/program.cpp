#pragma warning(disable : 26812)
#define FBXSDK_NAMESPACE fbxsdk
#include <fbxsdk.h>

// �w�肵��Vector4�̏���\������
void printfFbxVector4(const fbxsdk::FbxVector4& value) {
	printf("%1f, %1f, %1f, %1f\n",
		value[0], value[1], value[2], value[3]);
}
// �w�肵�����b�V���̏���\������
void PrintMesh(const fbxsdk::FbxMesh* mesh) {
	printf("AtttributeType: %s (controllPointsCount: %d)\n", "eMesh",
		mesh->GetControlPointsCount());

	//���_�����擾
	const auto controllPointsCount = mesh->GetControlPointsCount();
	// ���_�z������ׂĕ\��
	for (int index = 0;index < controllPointsCount; index++) {
		const auto controllPoint = mesh->GetControlPointAt(index);
		printf("controllPoint[%d] ", index);
		printfFbxVector4(controllPoint);
	}

	// �|���S�������擾
	const auto PolygonCount = mesh->GetPolygonCount();
	// PoligonVertices�̔z��̑��C���f�b�N�X�����擾
	const auto polygonVertexCount = mesh->GetPolygonVertexCount();

	// �S�Ẵ|���S�������[�v����
	for (int polygonIndex = 0; polygonIndex < PolygonCount; polygonIndex++) {
		// ��̃|���S�����\�����钸�_�����擾����
		const auto polygonSize = mesh->GetPolygonSize(polygonIndex);
		printf("PolygonSize: %d\n", polygonSize);

		// ��̃|���S�����\�����钸�_�C���f�b�N�X�����ׂĕ\��
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

// �w�肵���X�P���g���̏���\������
void PrintSkeleton(const fbxsdk::FbxSkeleton* skeleton) {
	printf("AttributeType: %s\n", "eSkeleton");
}

//�w�肵���m�[�h�A�g���r���[�g���󂯎���Ă��̏���\������
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

// �w�肵���m�[�h�̏���\������
void LoadNodeFromFbx(fbxsdk::FbxNode* node) {

	printf("%s :(childCount:%d, NodeAttributeCount: %d)\n",
		node->GetName(), node->GetChildCount(), node->GetNodeAttributeCount());
	//�m�[�h����NodeAttribute�����ׂČ���
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

	// Importer���쐬
	FbxImporter* importer = FbxImporter::Create(manager, "");
	if (importer == nullptr) {
		perror("Error: FbxImporter���쐬�ł��܂���ł���\n");
		exit(1);
	}

	// ���̓t�@�C�������w��
	if (!importer->Initialize("sample.fbx")) {
		// �w�肵���t�@�C�������݂��Ȃ�������G���[���o��
		perror("Error: FbxImporter���������ł��܂���B\n");
		manager->Destroy();
		exit(1);
	}

	// �C���|�[�g���s
	if (!importer->Import(scene)) {
		perror("Error:�V�[�����C���|�[�g�o���܂���B\n");
		manager->Destroy();
		exit(1);
	}

	//�����ŃV�[�����̃f�[�^����������
	scene->GetRootNode();

	// �V�[�������擾
	printf("�V�[����:%s (NodeCount: %d)\n", scene->GetName(), scene->GetNodeCount());

	// ���[�g�m�[�h���擾
	const auto rootnode = scene->GetRootNode();
	printf("���[�g�m�[�h: ");
	LoadNodeFromFbx(rootnode);


	//FBX Manager���J������
	manager->Destroy();

	return 0;
}
