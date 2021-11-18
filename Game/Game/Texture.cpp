#include "Texture.h"
#include <SDL.h>
#include <GL/glew.h>
#include <SOIL2.h>

Texture::Texture()
	: mHeight(0)
	, mWidth(0)
	, mTextureID(0)
{

}

Texture::~Texture()
{

}

bool Texture::Load(const std::string& fileName)
{
	int channels = 0;
	unsigned char* image = SOIL_load_image(		// �ε忡 �����ϸ� nullptr ��ȯ
		fileName.c_str(),	// ���� �̸�
		&mWidth,			// �ʺ� ����
		&mHeight,			// ���� ����
		&channels,			// ä�� �� ����
		SOIL_LOAD_AUTO		// �̹��� ���� Ÿ�� �Ǵ� �ڵ��̿�
	);

	if (image == nullptr)
	{
		SDL_Log("Failed to load image : %s\n", fileName.c_str());
		return false;
	}

	// �̹��� �ε忡 ���������� �̹����� RGB���� RGBA���� �˾Ƴ��� �Ѵ�. �̴� ä���� ���� ���� �˾Ƴ� �� �ִ�
	int format = GL_RGB;
	if (channels == 4)
	{
		format = GL_RGBA;
	}

	// ���� glGenTextures�� ����ؼ� OpenGL �ؽ�ó ������Ʈ�� �����Ѵ�
	// ���� glBindTexture�� �ؽ�ó�� Ȱ��ȭ�Ѵ�
	glGenTextures(1, &mTextureID); //(�ؽ�ó�� ����, �ؽ�ó�� ������ unsigned int �迭)

	// �ٸ� openGL ��ü��� ����������, ���ε� �ؼ� ���� �ؽ�ó ����� ���� �ؽ�ó�� ������� ������ �� �ֵ��� �Ѵ�
	// ��, ���� mTextureID ����� �ؽ�ó�� GL_TEXTURE_2D�� ���ε� �ߴٴ� �ǹ��̴�
	glBindTexture(GL_TEXTURE_2D, mTextureID);	// GL_TEXTURE_2D�� ���� �Ϲ����� �ؽ�ó Ÿ��������, ��� �ؽ�ó ������ ���� �ٸ� Ÿ�굵 ������

	// �ؽ�ó ������Ʈ�� ���� ����, glTexImage2D �Լ��� ���� �̹��� �����͸�
	// �ؽ�ó ������Ʈ�� �����ϸ� �ȴ�
	glTexImage2D(
		GL_TEXTURE_2D,		// �ؽ�ó Ÿ��. ���� GL_TEXTURE_2D�� ���ε� �� �ؽ�ó ��ü�� �ؽ�ó�� �����ϰڴٴ� �ǹ�
		0,					// LOD(������ 0) (mipmaps)
		format,				// OpenGL�� ����ؾ� �Ǵ� ���� ����
		mWidth,				// �ؽ�ó�� �ʺ�
		mHeight,			// �ؽ�ó�� ����
		0,					// ���� - "�� ���� 0�̾�� �Ѵ�"
		format,				// �Է� �������� ���� ����
		GL_UNSIGNED_BYTE,	// �Է� �������� ��Ʈ ����
							// Unsigned Byte�� 8��Ʈ ä���� �����Ѵ�
		image
	);

	// OpenGL �ؽ�ó ������Ʈ�� �̹��� �����͸� ����������, ���� SOIL�� �޸𸮻��� �̹��� ������ ������ �˸���
	SOIL_free_image_data(image);

	// ���������� glTexParameteri �� �̿��ؼ� ���� ���� ���͸��� Ȱ��ȭ��Ų��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// ������ �Ķ���͵��� 13�忡�� �����Ѵ�

	return true;
}

void Texture::UnLoad()
{
	glDeleteTextures(1, &mTextureID);
}

void Texture::CreateFromSurface(SDL_Surface* surface)
{
	mWidth = surface->w;
	mHeight = surface->h;

	// Generate a GL texture
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_BGRA,
		GL_UNSIGNED_BYTE, surface->pixels);

	// Use linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::SetActive()
{
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}