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
	// 이 텍스처의 OpenGL ID
	unsigned int mTextureID;

	// 너비 높이
	int mWidth;
	int mHeight;
};

