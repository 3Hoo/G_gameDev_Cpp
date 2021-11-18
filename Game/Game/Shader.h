#pragma once
#include <string>
#include <GL/glew.h>
#include <unordered_map>


/* OpenGL�� ���̴��� �ν��ϴ� ���� */
// 1. ���ؽ� ���̴� �ε�, ������
// 2. �����׸�Ʈ ���̴� �ε�, ������
// 3. 2���� ���̴��� "���̴� ���α׷�"�� ���� ����

class Shader
{
public:
	Shader();
	~Shader();

	// �־��� �̸����� ���ؽ�/�����׸�Ʈ ���̴� �ε�
	bool Load(const std::string& vertName, const std::string& fragName);

	// ���̴� ���α׷�, ���ؽ� ���̴�, �����׸�Ʈ ���̴��� ����
	void Unload();

	// �� ���̴��� Ȱ��ȭ�� ���̴� ���α׷����� ����
	void SetActive();

	// ���̴��� uniform ������ ���� �����Ѵ�
	void SetMatrixUniform(const char* name, const class Matrix4& matrix);
	void SetVectorUniform(const char* name, const class Vector3& vector);
	void SetFloatUniform(const char* name, const float f);

private:
	// ������ ���̴��� ����/������
	// GLuint�� �ܼ��� unsigned int�� OpenGL ����
	bool CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader);	// �������� ���̴� ������ �̸�, ���̴� Ÿ��, ���̴��� ID�� ������ ���� �Ķ����
	
	// ���̴��� ���������� �����ϵƴ��� Ȯ��
	bool IsCompiled(GLuint shader);

	// ���ؽ�/�����׸�Ʈ ���α׷��� ����ƴ��� Ȯ��
	bool IsValidProgram();

	// ���̴� ������Ʈ ID�� ����
	GLuint mVertexShader;
	GLuint mFragShader;
	GLuint mShaderProgram;

	// uniform ID�� �����صδ� ĳ��
	std::unordered_map<const char*, GLuint> mUniformIDs;
};

