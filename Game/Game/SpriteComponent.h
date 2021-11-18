#pragma once

#include "Component.h"
#include <SDL.h>

class SpriteComponent : public Component
{
public:
	// �׸��� �������� �������� �� ���ʿ��� �׸��� => ȭ�� �˰���
	SpriteComponent(class Actor* owner, int drawOrder = 100);
	~SpriteComponent();

	virtual void Draw(class Shader* renderer);
	virtual void SetTexture(class Texture* texture);
	class Texture* GetTexture() { return mTexture; }

	int GetDrawOrder() { return mDrawOrder; }
	int GetTexHeight() { return mTexHeight; }
	int GetTexWidth() { return mTexWidth; }

protected:
	// �׸� �ؽ�ó
	class Texture* mTexture;

	// ȭ�� �˰��򿡼� ����ϴ� �׸��� ����
	int mDrawOrder;

	// �ؽ�ó�� �ʺ�� ����
	int mTexWidth;
	int mTexHeight;
};

