//===================================
// Model.cpp
//
//===================================
#include "ModelViewer.h"
using namespace ModelViewer;
// ���̃N���X�̐V�����C���X�^���X���쐬���܂�
Model* Model::Create() {
	return new Model();
}
// �f�X�g���N�^�[
Model::~Model() {
	delete vertexBuffer;
	delete indexBuffer;
}
// ���̃��f���Ɋ܂܂��VertexBuffer���擾���܂�
VertexBuffer* Model::GetVertexBuffer() {
	return vertexBuffer;
}
// ���̃��f����VertexBuffer��ݒ肵�܂�
void Model::SetVertexBuffer(VertexBuffer* value) {
	vertexBuffer = value;
}
// ���̃��f���Ɋ܂܂��IndexBuffer���擾���܂�
IndexBuffer* Model::GetIndexBuffer() {
	return indexBuffer;
}
// ���̃��f����IndexBuffer��ݒ肵�܂�
void Model::SetIndexBuffer(IndexBuffer* value) {
	indexBuffer = value;
}
