#pragma once

#include "Component.h"
#include <SDL.h>

class SpriteComponent : public Component
{
public:
	// 그리기 순서값이 낮을수록 더 뒤쪽에서 그린다 => 화가 알고리즘
	SpriteComponent(class Actor* owner, int drawOrder = 100);
	~SpriteComponent();

	virtual void Draw(SDL_Renderer* renderer);
	virtual void SetTexture(SDL_Texture* texture);
	SDL_Texture* GetTexture() { return mTexture; }

	int GetDrawOrder() { return mDrawOrder; }
	int GetTexHeight() { return mTexHeight; }
	int GetTexWidth() { return mTexWidth; }

private:
	// 그릴 텍스처
	SDL_Texture* mTexture;

	// 화가 알고리즘에서 사용하는 그리기 순서
	int mDrawOrder;

	// 텍스처의 너비와 높이
	int mTexWidth;
	int mTexHeight;
};

