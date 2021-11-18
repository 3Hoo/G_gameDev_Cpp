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
	// TTF_OpenFont() �Լ��� .ttf ���Ϸκ��� Ư���� ũ��� ��Ʈ�� �ε�, �ش� ũ��� ��ġ�ϴ� TTF_Font ��Ʈ �������� �����͸� ��ȯ�Ѵ�.
	// TTF_OpenFont()�� ��Ʈ ũ�⸶�� �� ���� ȣ��Ǹ�, ������ ������ TTF_Font�� mFondData �ʿ� �߰��ȴ�

	// �ش� ��Ʈ ����� �����Ѵ�
	std::vector<int> fontSizes = { 8,9,10,11,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,52,56,60,64,68,72 };

	// �� ũ���� ��Ʈ ������� �� ���� TTF_OpenFont ȣ��
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
	// ������ ��Ʈ ũ�⸦ ����� �־��� ���ڿ��� '�ؽ�ó'�� ����
	Texture* texture = nullptr;

	// SDL_Color Ÿ������ ������ ��ȯ
	SDL_Color sdlColor;
	sdlColor.r = static_cast<Uint8>(color.x * 255);
	sdlColor.g = static_cast<Uint8>(color.y * 255);
	sdlColor.b = static_cast<Uint8>(color.z * 255);
	sdlColor.a = 255;

	// �ش� ũ���� ��Ʈ �˻�
	auto iter = mFontData.find(30);
	if (iter != mFontData.end())
	{
		TTF_Font* font = iter->second;
		// �ؽ�Ʈ�� �׸���(���İ����� ����)
		SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), sdlColor);
		if (surf != nullptr)
		{
			// SDL_Surface ��ü�� Texture ��ü�� ��ȯ�Ѵ�
			// TTF_RenderText_Blended �� SDL_Surface ��ü�� ��ȯ�ϴµ�, openGL�� SDL_Surface�� �׸��� ���Ѵ�. �׷��� Texture Ŭ������ ����Ѵ�
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