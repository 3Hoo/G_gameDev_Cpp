#include "VertexArray.h"
#include <GL/glew.h>
#include <GL/wglew.h>

VertexArray::VertexArray(const float* verts, unsigned int numVerts, const unsigned int* indicies, unsigned int numIndicies)
	: mNumVerts(numVerts)
	, mNumIndices(numIndicies)
{
	// 버텍스 배열 개체를 생성한 뒤(<GL/glew.h>), mVertexArray 멤버 변수에 ID를 저장한다
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);


	// 버텍스 버퍼를 생성한다. ID를 mVertexBuffer에 저장한다
	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);	// mVertexBuffer가 GL_ARRAY_BUFFER에 바인드

	// 버텍스 버퍼까지 생성했으니, 이제 VertexArray 생성자로 전달된 버텍스 데이터를 이 버텍스 버퍼에 복사한다
	// glBufferData를 사용한다
	glBufferData(
		GL_ARRAY_BUFFER,				// 데이터에 쓸 버퍼의 버퍼 타입 (mVertexBuffer가 GL_ARRAY_BUFFER에 바인드 되어있는 상태)		
		numVerts * 8 * sizeof(float),	// 복사할 '바이트' 크기. (x, y, z) (nx ny nz) (u v) 좌표를 사용하므로 *8을 한다
		verts,							// 복사할 소스(포인터)	
		GL_STATIC_DRAW					// 이 데이터를 어떻게 사용할 것인가? (삼각형을 그리는 방법). 데이터를 오직 한 번만 로드하며 버텍스가 자주 그려지는 경우에 사용되는 옵션
	);
	// => 개체 ID를 전달하는 것이 아니라, 현재 바인딩 될 버퍼의 타입을 지정하는 것에 주목하자! (개체 ID가 저장된 mVertexBuffer가 GL_ARRAY_BUFFER에 바인드 되었으므로, ID가 전달은 되는 듯)
	// => 이 경우에 GL_ARRAY_BUFFER는 막 생성한 버텍스 버퍼를 사용하겠다는 것을 의미한다! => 이 설명은 뭐 무시해도 될듯?


	// 인덱스 버퍼를 생성한다. ID를 mIndexBuffer에 저장한다
	// 차이점 : 인덱스 버퍼를 의미하는 GL_ELEMENT_ARRAY_BUFFER 버퍼 타입을 사용한다
	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);

	// 인덱스 데이터를 인덱스 버퍼에 복사한다
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		numIndicies * 1 * sizeof(unsigned int),
		indicies,
		GL_STATIC_DRAW
	);


	// 마지막으로 "버텍스 속성" 이라고도 불리는 버텍스 레이아웃을 지정해야 한다 => 버텍스 속성은 버텍스 어레이의 적절한 위치에 저장되어 있다 (x,y,z),(nx,ny,nz),(u,v)
	// 현재의 레이아웃은 3개의 float 값을 가진 위치다
	// 첫 번째 버텍스 속성(속성 0)을 활성화하기 위해 glEnableVertexAttribArray을 이용한다
	glEnableVertexAttribArray(0);
	// 그리고 크기, 타입, 속성의 포맷을 지정하기 위해 glVertexAttribPointer를 사용한다
	glVertexAttribPointer(
		0,					// 속성 인덱스 (첫 번째 버텍스 속성 인덱스는 0)
		3,					// 요소의 수 (이번에는 3. x, y, z)
		GL_FLOAT,			// 요소의 타입
		GL_FALSE,			// (정수형 타입에만 사용된다(관련있음))
		sizeof(float) * 8,	// 연속한 버텍스 사이의 바이트 오프셋 간격 (일반적으로 각 버텍스의 크기). 버텍스 버퍼에서 패딩이 없다면 간격은 버텍스의 크기가 된다.
		0					// 버텍스의 시작에서 이 속성까지의 오프셋 (위치 속성은 버텍스의 시작 위치와 동일하기 때문에 0으로 설정한다. Texture의 좌표는 4번째 부터 시작되므로 3 이겠지?)
	);

	// 두 번째 버텍스 속성 : 법선 벡터
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,					
		3,			
		GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 8,	// 간격은 항상 버텍스의 크기다
		reinterpret_cast<GLvoid*>(sizeof(float) * 3)	// 이 속성까지의 바이트 수는 sizeof(float)*3 이다. 하지만 OpenGL은 오프셋 포인터를 원한다. 그래서 reinterpret_cast로 void*로 강제변환 한다
	);

	// 세 번째 버텍스 속성 : 텍스처 uv
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
	// 버텍스 버퍼와 인덱스 버퍼, 버텍스 배열 객체를 해제한다
	glDeleteBuffers(1, &mVertexBuffer);
	glDeleteBuffers(1, &mIndexBuffer);
	glDeleteVertexArrays(1, &mVertexArray);
}

void VertexArray::SetActive()
{
	// 현재 사용할 버텍스 배열을 지정하는 glBindVertexArray를 호출한다
	glBindVertexArray(mVertexArray);
}