#pragma once

#include "SpriteComponent.h"
#include <vector>
#include "Math.h"

/*
* ����� ��游 ��ũ���ϴµ� ������ �����
* ���� ���� ����� ����� ȭ�� ũ���� �̹��� ���׸�Ʈ�� �����ϴ� ��
* �� �̹��� ���׸�Ʈ�� ��ũ���̶�� ȯ���� ����� ���� �����Ӹ��� ��ġ�� ����ȴ�
*/

class BGSpriteComponent : public SpriteComponent
{
public:
	// �⺻ �׸��� �������� ���� ������ ���� (����̹Ƿ�)
	BGSpriteComponent(class Actor* owner, int drawOrder = 10);

	// �θ�κ��� �������̵��� Update, Draw
	void Update(float deltaTime) override;
	void Draw(SDL_Renderer* renderer) override;

	// ��濡 ���Ǵ� �ؽ�ó ����
	void SetBGTextures(const std::vector<SDL_Texture*>& textures);

	// ȭ�� ũ�⸦ ��ų� ��ũ�� �ӵ����� ���� �� ��� �Լ�
	void SetScreenSize(const Vector2& size) { mScreenSize = size; }
	void SetScrollSpeed(float speed) { mScrollSpeed = speed; }
	float GetScrollSpeed() { return mScrollSpeed; }

private:
	// �� ��� �̹����� ��� �̹����� ������ ���� ĸ��ȭ�� ����ü
	// ������ ���� �ؽ�ó�� ��ġ�� �����Ӹ��� �����ϴ� �� ����Ѵ�
	struct BGTexture
	{
		SDL_Texture* mTexture;
		Vector2 mOffset;
	};

	std::vector<BGTexture> mBGTextures;
	Vector2 mScreenSize;
	float mScrollSpeed;
};
