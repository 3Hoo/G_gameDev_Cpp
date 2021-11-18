#pragma once
#include <string>
#include <GL/glew.h>
#include <unordered_map>


/* OpenGL이 셰이더를 인식하는 과정 */
// 1. 버텍스 셰이더 로드, 컴파일
// 2. 프래그먼트 셰이더 로드, 컴파일
// 3. 2개의 셰이더를 "셰이더 프로그램"에 서로 연결

class Shader
{
public:
	Shader();
	~Shader();

	// 주어진 이름으로 버텍스/프래그먼트 셰이더 로드
	bool Load(const std::string& vertName, const std::string& fragName);

	// 셰이더 프로그램, 버텍스 셰이더, 프래그먼트 셰이더를 삭제
	void Unload();

	// 이 셰이더를 활성화된 셰이더 프로그램으로 설정
	void SetActive();

	// 셰이더의 uniform 변수에 값을 전달한다
	void SetMatrixUniform(const char* name, const class Matrix4& matrix);
	void SetVectorUniform(const char* name, const class Vector3& vector);
	void SetFloatUniform(const char* name, const float f);

private:
	// 지정된 셰이더를 오픈/컴파일
	// GLuint는 단순히 unsigned int의 OpenGL 버전
	bool CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader);	// 컴파일할 셰이더 파일의 이름, 셰이더 타입, 셰이더의 ID를 저장할 참조 파라미터
	
	// 셰이더가 성공적으로 컴파일됐는지 확인
	bool IsCompiled(GLuint shader);

	// 버텍스/프래그먼트 프로그램이 연결됐는지 확인
	bool IsValidProgram();

	// 셰이더 오브젝트 ID를 저장
	GLuint mVertexShader;
	GLuint mFragShader;
	GLuint mShaderProgram;

	// uniform ID를 저장해두는 캐시
	std::unordered_map<const char*, GLuint> mUniformIDs;
};

