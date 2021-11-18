#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <SDL.h>
#include "Math.h"

Shader::Shader()
	: mShaderProgram(0)
	, mVertexShader(0)
	, mFragShader(0)
{

}

Shader::~Shader()
{

}

// 주어진 이름으로 버텍스/프래그먼트 셰이더 로드
bool Shader::Load(const std::string& vertName, const std::string& fragName)
{
	// 버텍스와 프래그먼트 셰이더 컴파일
	if (!CompileShader(vertName, GL_VERTEX_SHADER, mVertexShader) || !CompileShader(fragName, GL_FRAGMENT_SHADER, mFragShader))
	{
		return false;
	}

	// 1, 2단계 완료
	// 버텍스, 프래그먼트 셰이더를 서로 연결하는 셰이더 프로그램을 생성
	mShaderProgram = glCreateProgram();
	glAttachShader(mShaderProgram, mVertexShader);
	glAttachShader(mShaderProgram, mFragShader);
	glLinkProgram(mShaderProgram);

	// 프로그램이 성공적으로 연결됐는지 검증
	if (!IsValidProgram())
	{
		return false;
	}
	return true;
}

void Shader::Unload()
{
	glDeleteProgram(mShaderProgram);
	glDeleteShader(mVertexShader);
	glDeleteShader(mFragShader);
}

// 이 셰이더를 활성화된 셰이더 프로그램으로 설정
void Shader::SetActive()
{
	glUseProgram(mShaderProgram);
}

void Shader::SetMatrixUniform(const char* name, const Matrix4& matrix)
{
	// 해당 이름의 uniform을 찾는다
	GLuint loc;
	auto iter = mUniformIDs.find(name);
	if (iter != mUniformIDs.end())
	{
		loc = iter->second;
	}
	else
	{
		loc = glGetUniformLocation(mShaderProgram, name);
		mUniformIDs.emplace(name, loc);
	}
	//GLuint loc = glGetUniformLocation(mShaderProgram, name);

	// 행렬 데이터를 uniform에 전송한다
	glUniformMatrix4fv(
		loc,					// uniform ID
		1,						// 행렬의 수(이 경우는 오직 하나)	
		GL_TRUE,				// 행 벡터를 사용하면 TRUE로 설정한다
		matrix.GetAsFloatPtr()	// 행렬 데이터에 대한 포인터
	);
}

void Shader::SetVectorUniform(const char* name, const class Vector3& vector)
{
	// 해당 이름의 uniform을 찾는다
	GLuint loc;
	auto iter = mUniformIDs.find(name);
	if (iter != mUniformIDs.end())
	{
		loc = iter->second;
	}
	else
	{
		loc = glGetUniformLocation(mShaderProgram, name);
		mUniformIDs.emplace(name, loc);
	}

	glUniform3fv(
		loc,
		1,
		vector.GetAsFloatPtr()
	);
}

void Shader::SetFloatUniform(const char* name, const float f)
{
	// 해당 이름의 uniform을 찾는다
	GLuint loc;
	auto iter = mUniformIDs.find(name);
	if (iter != mUniformIDs.end())
	{
		loc = iter->second;
	}
	else
	{
		loc = glGetUniformLocation(mShaderProgram, name);
		mUniformIDs.emplace(name, loc);
	}

	glUniform1f(
		loc,
		f
	);
}


// 지정된 셰이더를 오픈/컴파일
bool Shader::CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader)
{
	// 파일을 연다
	std::ifstream shaderFile(fileName);
	if (shaderFile.is_open())
	{
		// 모든 텍스트를 읽어서 스트링으로 만든다
		std::stringstream sstream;
		sstream << shaderFile.rdbuf();
		std::string contents = sstream.str();
		const char* contentsChar = contents.c_str();

		// 지정된 타입의 셰이더를 생성한다
		outShader = glCreateShader(shaderType);
		// 소스 문자열을 설정하고 컴파일 시도
		glShaderSource(outShader, 1, &(contentsChar), nullptr);
		glCompileShader(outShader);

		if (!IsCompiled(outShader))
		{
			SDL_Log("Failed to compile shader %s", fileName.c_str());
			return false;
		}
	}
	else
	{
		SDL_Log("Shader file not found : %s", fileName.c_str());
		return false;
	}
	return true;
}

// 셰이더가 성공적으로 컴파일됐는지 확인
bool Shader::IsCompiled(GLuint shader)
{
	GLint status;
	// 컴파일 상태를 확인한다
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);	// 셰이더 컴파일 상태를 질의하며 함수는 정숫값으로 상태 코드를 반환한다.
	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512); // buffer라는 포인터 주소에 0이라는 값을 512바이트 채운다 (파라미터는 int형이지만, 실제로는 char형으로 저장)
		glGetShaderInfoLog(shader, 511, nullptr, buffer);	// 이 함수로 사람이 읽을 수 있는 컴파일 에러 메세지를 얻는다
		SDL_Log("GLSL compile Failed : \n%s", buffer);
		return false;
	}
	return true;
}

// 버텍스/프래그먼트 프로그램이 연결됐는지 확인
bool Shader::IsValidProgram()
{
	GLint status;
	// 컴파일 상태를 확인한다
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		glGetProgramInfoLog(mShaderProgram, 511, nullptr, buffer);
		SDL_Log("GLSL program link Status : \n%s", buffer);
		return false;
	}
	return true;
}

