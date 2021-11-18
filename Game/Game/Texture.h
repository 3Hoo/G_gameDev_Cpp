#pragma once

#include <string>

class Texture
{
public:
	Texture();
	~Texture();

	bool Load(const std::string& fileName);
	void UnLoad();

	void CreateFromSurface(struct SDL_Surface* surface);

	void SetActive();
	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }

private:
	// �� �ؽ�ó�� OpenGL ID
	unsigned int mTextureID;

	// �ʺ� ����
	int mWidth;
	int mHeight;
};

