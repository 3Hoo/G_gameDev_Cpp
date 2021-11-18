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
		// SpriteComponent�� �޸�, ������ ���� ��ȯ ����� ���� ����Ѵ� -> �߰����� �����ϸ��� �ʿ���� ����
		shader->SetMatrixUniform("uWorldTransform", mOwner->GetWorldTransform());

		// Ȱ��ȭ�� �ؽ�ó ����
		Texture* t = mMesh->GetTexture(mTextureIndex);
		if (t)
		{
			t->SetActive();
		}

		// Ȱ��ȭ�� �޽��� ���ؽ� �迭 ����
		VertexArray* va = mMesh->GetVertexArray();
		va->SetActive();

		// ���̴��� uSpecPower uniform �� ����
		shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());

		// �޽ø� �׸���
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
