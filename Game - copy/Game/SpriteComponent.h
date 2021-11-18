#pragma once

#include "Component.h"
#include <SDL.h>

class SpriteComponent : public Component
{
public:
	// �׸��� �������� �������� �� ���ʿ��� �׸��� => ȭ�� �˰���
	SpriteComponent(class Actor* owner, int drawOrder = 100);
	~SpriteComponent();

	virtual void Draw(SDL_Renderer* renderer);
	virtual void SetTexture(SDL_Texture* texture);
	SDL_Texture* GetTexture() { return mTexture; }

	int GetDrawOrder() { return mDrawOrder; }
	int GetTexHeight() { return mTexHeight; }
	int GetTexWidth() { return mTexWidth; }

private:
	// �׸� �ؽ�ó
	SDL_Texture* mTexture;

	// ȭ�� �˰��򿡼� ����ϴ� �׸��� ����
	int mDrawOrder;

	// �ؽ�ó�� �ʺ�� ����
	int mTexWidth;
	int mTexHeight;
};

