#include "MeshComponent.h"
#include "Actor.h"
#include "Mesh.h"
#include "Shader.h"
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"

MeshComponent::MeshComponent(Actor* owner)
	: Component(owner)
	, mTextureIndex(0)
	, mMesh(nullptr)
{
	mOwner->GetGame()->GetRenderer()->AddMeshComponent(this);
}

MeshComponent::~MeshComponent()
{
	mOwner->GetGame()->GetRenderer()->RemoveMeshComponent(this);
}


void MeshComponent::Draw(Shader* shader)
{
	if (mMesh)
	{
		// SpriteComponent와 달리, 액터의 세계 변환 행렬을 직접 사용한다 -> 추가적인 스케일링이 필요없기 때문
		shader->SetMatrixUniform("uWorldTransform", mOwner->GetWorldTransform());

		// 활성화될 텍스처 설정
		Texture* t = mMesh->GetTexture(mTextureIndex);
		if (t)
		{
			t->SetActive();
		}

		// 활성화될 메시의 버텍스 배열 설정
		VertexArray* va = mMesh->GetVertexArray();
		va->SetActive();

		// 셰이더에 uSpecPower uniform 값 설정
		shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());

		// 메시를 그린다
		glDrawElements(GL_TRIANGLES, va->GetNumIndicies(), GL_UNSIGNED_INT, nullptr);
	}
}


void MeshComponent::SetMesh(Mesh* mesh)
{
	mMesh = mesh;
	mShaderName = mMesh->GetShaderName();
}

Mesh* MeshComponent::GetMesh()
{
	return mMesh;
}

void MeshComponent::SetTextureIndex(size_t index)
{
	mTextureIndex = index;
}
