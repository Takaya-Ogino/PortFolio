//===================================
// Model.cpp
//
//===================================
#include "ModelViewer.h"
using namespace ModelViewer;
// このクラスの新しいインスタンスを作成します
Model* Model::Create() {
	return new Model();
}
// デストラクター
Model::~Model() {
	delete vertexBuffer;
	delete indexBuffer;
}
// このモデルに含まれるVertexBufferを取得します
VertexBuffer* Model::GetVertexBuffer() {
	return vertexBuffer;
}
// このモデルにVertexBufferを設定します
void Model::SetVertexBuffer(VertexBuffer* value) {
	vertexBuffer = value;
}
// このモデルに含まれるIndexBufferを取得します
IndexBuffer* Model::GetIndexBuffer() {
	return indexBuffer;
}
// このモデルにIndexBufferを設定します
void Model::SetIndexBuffer(IndexBuffer* value) {
	indexBuffer = value;
}
