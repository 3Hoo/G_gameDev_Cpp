#pragma once

class VertexArray
{
public:
	VertexArray(const float* verts, unsigned int numVerts, const unsigned int* indicies, unsigned int numIndicies);
	~VertexArray();

	// 이 버텍스 배열을 활성화 (활성화를 하면 이 버텍스 배열을 그릴 수 있다)
	void SetActive();

	unsigned int GetNumIndicies() const { return mNumIndices; }
	unsigned int GetNumVerts() const { return mNumVerts; }

private:
	// 버텍스 버퍼에 존재하는 버텍스의 수
	unsigned int mNumVerts;
	// 인덱스 버퍼에 존재하는 인덱스의 수
	unsigned int mNumIndices;

	/* 
	OpenGL은 생성한 개체의 포인터가 아니라, 정수형 ID를 반환한다 
	ID번호는 여러 유형의 개체에 대해서 고유하지 않다.
	그런데 OpenGL은 여러 유형의 개체를 동시에 사용하므로, 버텍스와 인덱스 버퍼 모두 ID가 1인 것도 가능하다!
	*/
	// 버텍스 버퍼의 OpenGL ID
	unsigned int mVertexBuffer;
	// 인덱스 버퍼의 OpenGL ID
	unsigned int mIndexBuffer;
	// 버텍스 배열 개체의 OpenGL ID
	unsigned int mVertexArray;
};

