#include "VertexArray.h"
#include <GL/glew.h>
#include <GL/wglew.h>

VertexArray::VertexArray(const float* verts, unsigned int numVerts, const unsigned int* indicies, unsigned int numIndicies)
	: mNumVerts(numVerts)
	, mNumIndices(numIndicies)
{
	// ���ؽ� �迭 ��ü�� ������ ��(<GL/glew.h>), mVertexArray ��� ������ ID�� �����Ѵ�
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);


	// ���ؽ� ���۸� �����Ѵ�. ID�� mVertexBuffer�� �����Ѵ�
	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);	// mVertexBuffer�� GL_ARRAY_BUFFER�� ���ε�

	// ���ؽ� ���۱��� ����������, ���� VertexArray �����ڷ� ���޵� ���ؽ� �����͸� �� ���ؽ� ���ۿ� �����Ѵ�
	// glBufferData�� ����Ѵ�
	glBufferData(
		GL_ARRAY_BUFFER,				// �����Ϳ� �� ������ ���� Ÿ�� (mVertexBuffer�� GL_ARRAY_BUFFER�� ���ε� �Ǿ��ִ� ����)		
		numVerts * 8 * sizeof(float),	// ������ '����Ʈ' ũ��. (x, y, z) (nx ny nz) (u v) ��ǥ�� ����ϹǷ� *8�� �Ѵ�
		verts,							// ������ �ҽ�(������)	
		GL_STATIC_DRAW					// �� �����͸� ��� ����� ���ΰ�? (�ﰢ���� �׸��� ���). �����͸� ���� �� ���� �ε��ϸ� ���ؽ��� ���� �׷����� ��쿡 ���Ǵ� �ɼ�
	);
	// => ��ü ID�� �����ϴ� ���� �ƴ϶�, ���� ���ε� �� ������ Ÿ���� �����ϴ� �Ϳ� �ָ�����! (��ü ID�� ����� mVertexBuffer�� GL_ARRAY_BUFFER�� ���ε� �Ǿ����Ƿ�, ID�� ������ �Ǵ� ��)
	// => �� ��쿡 GL_ARRAY_BUFFER�� �� ������ ���ؽ� ���۸� ����ϰڴٴ� ���� �ǹ��Ѵ�! => �� ������ �� �����ص� �ɵ�?


	// �ε��� ���۸� �����Ѵ�. ID�� mIndexBuffer�� �����Ѵ�
	// ������ : �ε��� ���۸� �ǹ��ϴ� GL_ELEMENT_ARRAY_BUFFER ���� Ÿ���� ����Ѵ�
	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);

	// �ε��� �����͸� �ε��� ���ۿ� �����Ѵ�
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		numIndicies * 1 * sizeof(unsigned int),
		indicies,
		GL_STATIC_DRAW
	);


	// ���������� "���ؽ� �Ӽ�" �̶�� �Ҹ��� ���ؽ� ���̾ƿ��� �����ؾ� �Ѵ� => ���ؽ� �Ӽ��� ���ؽ� ����� ������ ��ġ�� ����Ǿ� �ִ� (x,y,z),(nx,ny,nz),(u,v)
	// ������ ���̾ƿ��� 3���� float ���� ���� ��ġ��
	// ù ��° ���ؽ� �Ӽ�(�Ӽ� 0)�� Ȱ��ȭ�ϱ� ���� glEnableVertexAttribArray�� �̿��Ѵ�
	glEnableVertexAttribArray(0);
	// �׸��� ũ��, Ÿ��, �Ӽ��� ������ �����ϱ� ���� glVertexAttribPointer�� ����Ѵ�
	glVertexAttribPointer(
		0,					// �Ӽ� �ε��� (ù ��° ���ؽ� �Ӽ� �ε����� 0)
		3,					// ����� �� (�̹����� 3. x, y, z)
		GL_FLOAT,			// ����� Ÿ��
		GL_FALSE,			// (������ Ÿ�Կ��� ���ȴ�(��������))
		sizeof(float) * 8,	// ������ ���ؽ� ������ ����Ʈ ������ ���� (�Ϲ������� �� ���ؽ��� ũ��). ���ؽ� ���ۿ��� �е��� ���ٸ� ������ ���ؽ��� ũ�Ⱑ �ȴ�.
		0					// ���ؽ��� ���ۿ��� �� �Ӽ������� ������ (��ġ �Ӽ��� ���ؽ��� ���� ��ġ�� �����ϱ� ������ 0���� �����Ѵ�. Texture�� ��ǥ�� 4��° ���� ���۵ǹǷ� 3 �̰���?)
	);

	// �� ��° ���ؽ� �Ӽ� : ���� ����
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,					
		3,			
		GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 8,	// ������ �׻� ���ؽ��� ũ���
		reinterpret_cast<GLvoid*>(sizeof(float) * 3)	// �� �Ӽ������� ����Ʈ ���� sizeof(float)*3 �̴�. ������ OpenGL�� ������ �����͸� ���Ѵ�. �׷��� reinterpret_cast�� void*�� ������ȯ �Ѵ�
	);

	// �� ��° ���ؽ� �Ӽ� : �ؽ�ó uv
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 8,
		reinterpret_cast<GLvoid*>(sizeof(float) * 6)
	);
}


VertexArray::~VertexArray()
{
	// ���ؽ� ���ۿ� �ε��� ����, ���ؽ� �迭 ��ü�� �����Ѵ�
	glDeleteBuffers(1, &mVertexBuffer);
	glDeleteBuffers(1, &mIndexBuffer);
	glDeleteVertexArrays(1, &mVertexArray);
}

void VertexArray::SetActive()
{
	// ���� ����� ���ؽ� �迭�� �����ϴ� glBindVertexArray�� ȣ���Ѵ�
	glBindVertexArray(mVertexArray);
}