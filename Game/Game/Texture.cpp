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
	unsigned char* image = SOIL_load_image(		// 로드에 실패하면 nullptr 반환
		fileName.c_str(),	// 파일 이름
		&mWidth,			// 너비 저장
		&mHeight,			// 높이 저장
		&channels,			// 채널 수 저장
		SOIL_LOAD_AUTO		// 이미지 파일 타입 또는 자동이요
	);

	if (image == nullptr)
	{
		SDL_Log("Failed to load image : %s\n", fileName.c_str());
		return false;
	}

	// 이미지 로드에 성공했으면 이미지가 RGB인지 RGBA인지 알아내야 한다. 이는 채널의 수를 토대로 알아낼 수 있다
	int format = GL_RGB;
	if (channels == 4)
	{
		format = GL_RGBA;
	}

	// 이제 glGenTextures를 사용해서 OpenGL 텍스처 오브젝트를 생성한다
	// 이후 glBindTexture로 텍스처를 활성화한다
	glGenTextures(1, &mTextureID); //(텍스처의 개수, 텍스처를 저장할 unsigned int 배열)

	// 다른 openGL 객체들과 마찬가지로, 바인딩 해서 각종 텍스처 명령이 현재 텍스처를 대상으로 설정할 수 있도록 한다
	// 즉, 이제 mTextureID 저장된 텍스처를 GL_TEXTURE_2D에 바인드 했다는 의미이다
	glBindTexture(GL_TEXTURE_2D, mTextureID);	// GL_TEXTURE_2D는 가장 일반적인 텍스처 타깃이지만, 고급 텍스처 유형을 위한 다른 타깃도 존재함

	// 텍스처 오브젝트를 얻은 이후, glTexImage2D 함수로 원본 이미지 데이터를
	// 텍스처 오브젝트에 복사하면 된다
	glTexImage2D(
		GL_TEXTURE_2D,		// 텍스처 타깃. 현재 GL_TEXTURE_2D로 바인딩 된 텍스처 객체에 텍스처를 생성하겠다는 의미
		0,					// LOD(지금은 0) (mipmaps)
		format,				// OpenGL이 사용해야 되는 색상 포맷
		mWidth,				// 텍스처의 너비
		mHeight,			// 텍스처의 높이
		0,					// 보더 - "이 값은 0이어야 한다"
		format,				// 입력 데이터의 색상 포맷
		GL_UNSIGNED_BYTE,	// 입력 데이터의 비트 깊이
							// Unsigned Byte는 8비트 채널을 지정한다
		image
	);

	// OpenGL 텍스처 오브젝트에 이미지 데이터를 복사했으니, 이제 SOIL에 메모리상의 이미지 데이터 해제를 알린다
	SOIL_free_image_data(image);

	// 마지막으로 glTexParameteri 를 이용해서 이중 선형 필터링을 활성화시킨다
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// 여기의 파라미터들은 13장에서 설명한다

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