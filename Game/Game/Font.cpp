#include "Font.h"
#include "Texture.h"
#include "Game.h"
#include <vector>

Font::Font(Game* game)
	: mGame(game)
{

}

Font::~Font()
{

}

bool Font::Load(const std::string& fileName)
{
	// TTF_OpenFont() 함수는 .ttf 파일로부터 특정한 크기로 폰트를 로드, 해당 크기와 일치하는 TTF_Font 폰트 데이터의 포인터를 반환한다.
	// TTF_OpenFont()는 폰트 크기마다 한 번씩 호출되며, 생성한 각각의 TTF_Font는 mFondData 맵에 추가된다

	// 해당 폰트 사이즈를 지원한다
	std::vector<int> fontSizes = { 8,9,10,11,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,52,56,60,64,68,72 };

	// 각 크기의 폰트 사이즈마다 한 번씩 TTF_OpenFont 호출
	for (auto& size : fontSizes)
	{
		TTF_Font* font = TTF_OpenFont(fileName.c_str(), size);
		if (font == nullptr)
		{
			SDL_Log("Failed to load font %s in size %d", fileName.c_str(), size);
			return false;
		}
		mFontData.emplace(size, font);
	}
	return true;
}

void Font::Unload()
{

}

Texture* Font::RenderText(const std::string& text, const Vector3& color, int pointSize)
{
	// 적당한 폰트 크기를 사용해 주어진 문자열의 '텍스처'를 생성
	Texture* texture = nullptr;

	// SDL_Color 타입으로 색상을 변환
	SDL_Color sdlColor;
	sdlColor.r = static_cast<Uint8>(color.x * 255);
	sdlColor.g = static_cast<Uint8>(color.y * 255);
	sdlColor.b = static_cast<Uint8>(color.z * 255);
	sdlColor.a = 255;

	// 해당 크기의 폰트 검색
	auto iter = mFontData.find(30);
	if (iter != mFontData.end())
	{
		TTF_Font* font = iter->second;
		// 텍스트를 그린다(알파값으로 블렌딩)
		SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), sdlColor);
		if (surf != nullptr)
		{
			// SDL_Surface 객체를 Texture 객체로 변환한다
			// TTF_RenderText_Blended 는 SDL_Surface 객체를 반환하는데, openGL은 SDL_Surface를 그리지 못한다. 그래서 Texture 클래스를 사용한다
			texture = new Texture();
			texture->CreateFromSurface(surf);
		}
	}
	else
	{
		SDL_Log("Point size %d is unsupported", pointSize);
	}
	return texture;
}