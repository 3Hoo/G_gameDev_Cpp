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

// �־��� �̸����� ���ؽ�/�����׸�Ʈ ���̴� �ε�
bool Shader::Load(const std::string& vertName, const std::string& fragName)
{
	// ���ؽ��� �����׸�Ʈ ���̴� ������
	if (!CompileShader(vertName, GL_VERTEX_SHADER, mVertexShader) || !CompileShader(fragName, GL_FRAGMENT_SHADER, mFragShader))
	{
		return false;
	}

	// 1, 2�ܰ� �Ϸ�
	// ���ؽ�, �����׸�Ʈ ���̴��� ���� �����ϴ� ���̴� ���α׷��� ����
	mShaderProgram = glCreateProgram();
	glAttachShader(mShaderProgram, mVertexShader);
	glAttachShader(mShaderProgram, mFragShader);
	glLinkProgram(mShaderProgram);

	// ���α׷��� ���������� ����ƴ��� ����
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

// �� ���̴��� Ȱ��ȭ�� ���̴� ���α׷����� ����
void Shader::SetActive()
{
	glUseProgram(mShaderProgram);
}

void Shader::SetMatrixUniform(const char* name, const Matrix4& matrix)
{
	// �ش� �̸��� uniform�� ã�´�
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

	// ��� �����͸� uniform�� �����Ѵ�
	glUniformMatrix4fv(
		loc,					// uniform ID
		1,						// ����� ��(�� ���� ���� �ϳ�)	
		GL_TRUE,				// �� ���͸� ����ϸ� TRUE�� �����Ѵ�
		matrix.GetAsFloatPtr()	// ��� �����Ϳ� ���� ������
	);
}

void Shader::SetVectorUniform(const char* name, const class Vector3& vector)
{
	// �ش� �̸��� uniform�� ã�´�
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
	// �ش� �̸��� uniform�� ã�´�
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


// ������ ���̴��� ����/������
bool Shader::CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader)
{
	// ������ ����
	std::ifstream shaderFile(fileName);
	if (shaderFile.is_open())
	{
		// ��� �ؽ�Ʈ�� �о ��Ʈ������ �����
		std::stringstream sstream;
		sstream << shaderFile.rdbuf();
		std::string contents = sstream.str();
		const char* contentsChar = contents.c_str();

		// ������ Ÿ���� ���̴��� �����Ѵ�
		outShader = glCreateShader(shaderType);
		// �ҽ� ���ڿ��� �����ϰ� ������ �õ�
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

// ���̴��� ���������� �����ϵƴ��� Ȯ��
bool Shader::IsCompiled(GLuint shader)
{
	GLint status;
	// ������ ���¸� Ȯ���Ѵ�
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);	// ���̴� ������ ���¸� �����ϸ� �Լ��� ���������� ���� �ڵ带 ��ȯ�Ѵ�.
	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512); // buffer��� ������ �ּҿ� 0�̶�� ���� 512����Ʈ ä��� (�Ķ���ʹ� int��������, �����δ� char������ ����)
		glGetShaderInfoLog(shader, 511, nullptr, buffer);	// �� �Լ��� ����� ���� �� �ִ� ������ ���� �޼����� ��´�
		SDL_Log("GLSL compile Failed : \n%s", buffer);
		return false;
	}
	return true;
}

// ���ؽ�/�����׸�Ʈ ���α׷��� ����ƴ��� Ȯ��
bool Shader::IsValidProgram()
{
	GLint status;
	// ������ ���¸� Ȯ���Ѵ�
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

