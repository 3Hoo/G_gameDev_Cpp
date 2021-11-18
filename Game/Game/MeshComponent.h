#pragma once

#include "Component.h"
#include <string>

class MeshComponent : public Component
{
public:
	MeshComponent(class Actor* owner);
	~MeshComponent();

	// 제공된 셰이더로 이 메시 컴포넌트를 그린다
	virtual void Draw(class Shader* shader);

	// 메시 컴포넌트가 사용하는 메시, 텍스처를 설정
	virtual void SetMesh(class Mesh* mesh);
	class Mesh* GetMesh();
	void SetTextureIndex(size_t index);

	const std::string& GetShaderName() const { return mShaderName; }

protected:
	class Mesh* mMesh;

	std::string mShaderName;

	// Mesh의 Json 파일에는 textures가 리스트로 되어있다. 이 index는 그 리스트 중 어느 텍스처를 사용할 것인지 알려준다
	size_t mTextureIndex;
};

