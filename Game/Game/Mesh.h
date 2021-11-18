#pragma once

#include <string>
#include <vector>
#include "Collision.h"

class Mesh
{
public:
	Mesh();
	~Mesh();

	// �޽� �ε�, ��ε�
	// renderer�� ��� �ؽ�ó���� ���� �����Ƿ�, �̸� �������� ���� renderer�� ���Խ�Ų��
	bool Load(const std::string& fileName, class Renderer* renderer);
	void Unload();

	// �� �޽ÿ� �ش��ϴ� ���ؽ� �迭�� ��´�
	class VertexArray* GetVertexArray() { return mVertexArray; }

	// Ư�� �ε����� �ش��ϴ� �ؽ�ó�� ��´�
	class Texture* GetTexture(size_t index);

	// ���̴��� �̸��� ��´�
	const std::string& GetShaderName() const { return mShaderName; }

	// ������Ʈ ���� �ٿ�� ��ü �������� ��´�
	float GetRadius() const { return mRadius; }

	// (���ݻ�)specular power ���� ��´�
	float GetSpecPower() const { return mSpecPower; }

	const AABB& GetBox() const { return mBox; }

private:
	// �� �޽ÿ� ���Ǵ� �ؽ�ó��
	std::vector<class Texture*> mTextures;

	// �޽��� ���ؽ� �迭
	class VertexArray* mVertexArray;

	// �޽ð� ������ ���̴� �̸�
	std::string mShaderName;

	// ������Ʈ ���� �ٿ�� ��ü�� �������� = ������Ʈ ������ ������ ���� �ָ� ������ ������ �Ÿ�
	float mRadius;

	// Specular Power
	float mSpecPower;

	// �ٿ��
	AABB mBox;
};

