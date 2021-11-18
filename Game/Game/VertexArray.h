#pragma once

class VertexArray
{
public:
	VertexArray(const float* verts, unsigned int numVerts, const unsigned int* indicies, unsigned int numIndicies);
	~VertexArray();

	// �� ���ؽ� �迭�� Ȱ��ȭ (Ȱ��ȭ�� �ϸ� �� ���ؽ� �迭�� �׸� �� �ִ�)
	void SetActive();

	unsigned int GetNumIndicies() const { return mNumIndices; }
	unsigned int GetNumVerts() const { return mNumVerts; }

private:
	// ���ؽ� ���ۿ� �����ϴ� ���ؽ��� ��
	unsigned int mNumVerts;
	// �ε��� ���ۿ� �����ϴ� �ε����� ��
	unsigned int mNumIndices;

	/* 
	OpenGL�� ������ ��ü�� �����Ͱ� �ƴ϶�, ������ ID�� ��ȯ�Ѵ� 
	ID��ȣ�� ���� ������ ��ü�� ���ؼ� �������� �ʴ�.
	�׷��� OpenGL�� ���� ������ ��ü�� ���ÿ� ����ϹǷ�, ���ؽ��� �ε��� ���� ��� ID�� 1�� �͵� �����ϴ�!
	*/
	// ���ؽ� ������ OpenGL ID
	unsigned int mVertexBuffer;
	// �ε��� ������ OpenGL ID
	unsigned int mIndexBuffer;
	// ���ؽ� �迭 ��ü�� OpenGL ID
	unsigned int mVertexArray;
};

