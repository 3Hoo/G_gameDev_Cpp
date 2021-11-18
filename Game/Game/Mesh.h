#pragma once

#include <string>
#include <vector>
#include "Collision.h"

class Mesh
{
public:
	Mesh();
	~Mesh();

	// 메시 로드, 언로드
	// renderer이 모든 텍스처들을 갖고 있으므로, 이를 가져오기 위해 renderer을 포함시킨다
	bool Load(const std::string& fileName, class Renderer* renderer);
	void Unload();

	// 이 메시에 해당하는 버텍스 배열을 얻는다
	class VertexArray* GetVertexArray() { return mVertexArray; }

	// 특정 인덱스에 해당하는 텍스처를 얻는다
	class Texture* GetTexture(size_t index);

	// 셰이더의 이름을 얻는다
	const std::string& GetShaderName() const { return mShaderName; }

	// 오브젝트 공간 바운딩 구체 반지름을 얻는다
	float GetRadius() const { return mRadius; }

	// (정반사)specular power 값을 얻는다
	float GetSpecPower() const { return mSpecPower; }

	const AABB& GetBox() const { return mBox; }

private:
	// 이 메시에 사용되는 텍스처들
	std::vector<class Texture*> mTextures;

	// 메시의 버텍스 배열
	class VertexArray* mVertexArray;

	// 메시가 지정한 셰이더 이름
	std::string mShaderName;

	// 오브젝트 공간 바운딩 구체의 반지름값 = 오브젝트 공간의 원점과 가장 멀리 떨어진 점과의 거리
	float mRadius;

	// Specular Power
	float mSpecPower;

	// 바운딩
	AABB mBox;
};

