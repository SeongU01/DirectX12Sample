#include "pch.h"
#include "Model.h"
// TODO: Model 클래스는 현재 BaseMesh의 집합체로만 존재하지만, 향후에는 애니메이션, 머티리얼, 텍스처 등 다양한 요소를
// 포함할 수 있도록 확장될 수 있습니다.
Model::Model() {}

Model::~Model() {}

void Model::AddMesh(std::unique_ptr<BaseMesh> mesh)
{
    _meshes.emplace_back(std::move(mesh));
}
