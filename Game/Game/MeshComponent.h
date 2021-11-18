#pragma once

#include "Component.h"
#include <string>

class MeshComponent : public Component
{
public:
	MeshComponent(class Actor* owner);
	~MeshComponent();

	// ������ ���̴��� �� �޽� ������Ʈ�� �׸���
	virtual void Draw(class Shader* shader);

	// �޽� ������Ʈ�� ����ϴ� �޽�, �ؽ�ó�� ����
	virtual void SetMesh(class Mesh* mesh);
	class Mesh* GetMesh();
	void SetTextureIndex(size_t index);

	const std::string& GetShaderName() const { return mShaderName; }

protected:
	class Mesh* mMesh;

	std::string mShaderName;

	// Mesh�� Json ���Ͽ��� textures�� ����Ʈ�� �Ǿ��ִ�. �� index�� �� ����Ʈ �� ��� �ؽ�ó�� ����� ������ �˷��ش�
	size_t mTextureIndex;
};

