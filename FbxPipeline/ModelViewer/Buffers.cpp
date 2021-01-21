//=================================
// Buffers.cpp
//
//=================================
#include "ModelViewer.h"
using namespace std;
using namespace DirectX;
using namespace ModelViewer;

// ���t�@�C�����ł̂݌Ăяo���\
namespace {
	// �w�肵�����_���W��FbxVector4����XMFloat3�ɃL���X�g����
	XMFLOAT3 FbxVector4ToXMFloat3(const fbxsdk::FbxVector4& controllPoint) {
		XMFLOAT3 vertexPosition;
		vertexPosition.x = static_cast<float>(controllPoint[0]);
		vertexPosition.y = static_cast<float>(controllPoint[1]);
		vertexPosition.z = static_cast<float>(controllPoint[2]);
		return vertexPosition;
	}

	// �w�肵�����b�V���̏���\������
	void PrintMesh(const fbxsdk::FbxMesh* mesh, Model* model, ID3D11Device* graphicsDevice) {
		// VertexBuffer�N���X�̃C���X�^���X���쐬����
		model->SetVertexBuffer(VertexBuffer::Create(mesh, graphicsDevice));
		// IndexBuffer�̃N���X�̃C���X�^���X���쐬����
		model->SetIndexBuffer(IndexBuffer::Create(mesh, graphicsDevice));
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
	void PrintNodeAttribute(fbxsdk::FbxNodeAttribute* nodeAttribute, Model* model, ID3D11Device* graphicsDevice) {
		printf("%s: (Typename: %s)\n",
			nodeAttribute->GetName(), nodeAttribute->GetTypeName());

		const auto attributeType = nodeAttribute->GetAttributeType();
		// attributeType�ɉ����ĕ��򏈗�
		switch (attributeType) {
		case FbxNodeAttribute::eMesh:
			PrintMesh(static_cast<const fbxsdk::FbxMesh*>(nodeAttribute), model, graphicsDevice);
			break;
		case FbxNodeAttribute::eSkeleton:
			PrintSkeleton(static_cast<const fbxsdk::FbxSkeleton*>(nodeAttribute));
			break;
		}
	}

	// �w�肵���m�[�h�̏���\������
	void PrintNode(fbxsdk::FbxNode* node, Model* model, ID3D11Device* graphicsDevice) {

		//�m�[�h����NodeAttribute�����ׂČ���
		for (int index = 0; index < node->GetNodeAttributeCount(); index++) {
			const auto nodeAttribute = node->GetNodeAttributeByIndex(index);
			PrintNodeAttribute(nodeAttribute, model, graphicsDevice);
		}
		// �q�m�[�h�ֈڍs
		for (int index = 0; index < node->GetChildCount(); index++) {
			const auto childNode = node->GetChild(index);
			PrintNode(childNode, model, graphicsDevice);
		}
	}
}

// ���̃N���X�̃C���X�^���X���쐬���܂�
VertexBuffer* VertexBuffer::Create(const fbxsdk::FbxMesh* mesh, ID3D11Device* graphicsDevice) {
	// ���̃N���X�̃������[���m��
	VertexBuffer* vertexBuffer = new VertexBuffer(graphicsDevice, sizeof(VertexPosition), mesh->GetControlPointsCount());

	// ���_�z����m��
	vector<VertexPosition> vertices(vertexBuffer->GetVertexCount());
	// ���_�z������ׂ�FbxVector4����XMFloat3�փL���X�g
	for (int index = 0;index < vertexBuffer->vertexCount; index++) {
		const auto controllPoint = mesh->GetControlPointAt(index);
		vertices[index].position = FbxVector4ToXMFloat3(controllPoint);
	}
	// �o�b�t�@�[�Ƀf�[�^��]��
	vertexBuffer->SetData(vertices);

	return vertexBuffer;
}
// �R���X�g���N�^�[
VertexBuffer::VertexBuffer(ID3D11Device* graphicsDevice, UINT vertexStride, UINT vertexCount) {
	this->graphicsDevice = graphicsDevice;
	this->vertexCount = vertexCount;

	// �쐬���钸�_�o�b�t�@�[�ɂ��Ă̋L�q
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = vertexStride * vertexCount;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	// ���_�o�b�t�@�[���쐬
	const auto hr = graphicsDevice->CreateBuffer(&bufferDesc, NULL, &buffer);
	if (FAILED(hr)) {
		OutputDebugString(L"���_�o�b�t�@�[���쐬�ł��܂���ł����B");
	}
}
// �f�X�g���N�^�[
VertexBuffer::~VertexBuffer() {
	buffer->Release();
}
// ���_�����擾
int VertexBuffer::GetVertexCount() {
	return vertexCount;
}
// ���̃��\�[�X���쐬�����O���t�B�b�N�X�f�o�C�X���擾���܂�
ID3D11Device* ModelViewer::VertexBuffer::GetGraphicsDevice() {
	return graphicsDevice;
}
// �l�C�e�B�u�|�C���^�[���擾
ID3D11Buffer* VertexBuffer::GetNativePointer() {
	return buffer;
}

// ���̃N���X�̃C���X�^���X���쐬���܂�
IndexBuffer* IndexBuffer::Create(const fbxsdk::FbxMesh* mesh, ID3D11Device* graphicsDevice) {
	// ���̃N���X�̃������[���m��
	IndexBuffer* indexBuffer = new IndexBuffer(graphicsDevice, DXGI_FORMAT_R32_UINT, mesh->GetControlPointsCount());

	// �C���f�b�N�X�z����m��
	vector<uint32_t> indices;
	indices.reserve(indexBuffer->indexCount);
	// �|���S�������擾
	const auto polygonCount = mesh->GetPolygonCount();
	// �S�Ẵ|���S�������[�v����
	for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++) {
		// ��̃|���S�����\�����钸�_�����擾����
		const auto poligonSize = mesh->GetPolygonSize(polygonIndex);
		// ��̃|���S�����\�����钸�_�C���f�b�N�X�����ׂĕ\��
		for (int positionInPolygon = 0; positionInPolygon < poligonSize; positionInPolygon++) {
			const auto polygonVertex = mesh->GetPolygonVertex(polygonIndex, positionInPolygon);
			indices.push_back(polygonVertex);
		}
	}
	// �o�b�t�@�[�Ƀf�[�^��]��
	indexBuffer->SetData(indices);

	return indexBuffer;
}
// ���̃N���X�̐V�����C���X�^���X�����������܂�
IndexBuffer::IndexBuffer(ID3D11Device* graphicsDevice, DXGI_FORMAT indexElementSize, UINT indexCount) {
	this->graphicsDevice = graphicsDevice;
	this->indexCount = indexCount;
	this->indexElementSize = indexElementSize;

	// DXGI_FORMAT����v�f�̃T�C�Y�i�o�C�g���j�֕ϊ�
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
		OutputDebugString(TEXT("�֐��������Ȉ����ŌĂяo����܂���"));
		break;
	}
	// �쐬����C���f�b�N�X�o�b�t�@�[�ɂ��Ă̋L�q
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = indexCount * indexElementSize;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	// �C���f�b�N�X�o�b�t�@�[���쐬
	const auto hr = graphicsDevice->CreateBuffer(&bufferDesc, NULL, &buffer);
	if (FAILED(hr)) {
		OutputDebugString(L"�C���f�b�N�X�o�b�t�@�[���쐬�ł��܂���ł����B");
	}
}
// �f�X�g���N�^�[
IndexBuffer::~IndexBuffer() {
	buffer->Release();
}
// �C���f�b�N�X�����擾���܂�
int IndexBuffer::GetIndexCount() {
	return indexCount;
}
// ���̃C���f�b�N�X�o�b�t�@�[�Ɋi�[����Ă���X�̃C���f�b�N�X�̃T�C�Y�t�H�[�}�b�g���擾���܂�
DXGI_FORMAT IndexBuffer::GetIndexElementSize() {
	return indexElementSize;
}
// ���̃��\�[�X���쐬�����O���t�B�b�N�X�f�o�C�X���擾���܂�
ID3D11Device* IndexBuffer::GetGraphicsDevice() {
	return graphicsDevice;
}
// �l�C�e�B�u�|�C���^�[���擾���܂�
ID3D11Buffer* IndexBuffer::GetNativePointer() {
	return buffer;
}

// �w�肵��Fbx�t�@�C����ǂݍ��݂܂�
Model* ModelViewer::LoadFbx(const std::string& fileName, ID3D11Device* graphicsDevice) {

	//	FBX Manager���쐬
	const auto manager = FbxManager::Create();
	if (manager == nullptr) {
		perror("FBX MANAGER���쐬�ł��܂���ł���\n");
		return nullptr;
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
		return nullptr;
	}
	// ���̓t�@�C�������w��
	if (!importer->Initialize(fileName.c_str())) {
		// �w�肵���t�@�C�������݂��Ȃ�������G���[���o��
		perror("Error: FbxImporter���������ł��܂���B\n");
		manager->Destroy();
		return nullptr;
	}
	// �C���|�[�g���s
	if (!importer->Import(scene)) {
		perror("Error:�V�[�����C���|�[�g�o���܂���B\n");
		manager->Destroy();
		return nullptr;
	}

	const auto model = Model::Create();
	// ���[�g�m�[�h���擾����
	const auto rootNode = scene->GetRootNode();
	PrintNode(rootNode, model, graphicsDevice);

	HRESULT hr = S_OK;

	//�J������
	manager->Destroy();

	return model;
}
