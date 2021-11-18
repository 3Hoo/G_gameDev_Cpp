
#include "SpriteComponent.h"
#include "Actor.h"
#include "Game.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder)
	: Component(owner)
	, mTexture(nullptr)
	, mDrawOrder(drawOrder)
	, mTexHeight(0)
	, mTexWidth(0)
{
	mOwner->GetGame()->AddSprite(this);
}

SpriteComponent::~SpriteComponent()
{
	mOwner->GetGame()->RemoveSprite(this);
}

void SpriteComponent::Draw(SDL_Renderer* renderer)
{
	/*
	* ���⼭�� ������ ��ġ�� ȭ����� ���� ��ġ�� ��ġ�Ѵٰ� �����Ѵ�
	* ���� ������ ������ó�� ȭ���� �����̸鼭 �ɸ��Ͱ� �̵��ϴ� ��쿡�� ����� �� ���� ���.
	* �̸� �����Ϸ��� "ī�޶�"�� ������ �ʿ��ϴ� => 9��
	*/
	if (mTexture)
	{
		SDL_Rect r;

		// �������� ����(������) ������ �ʺ�, ���̸� ������ �Ѵ�
		r.w = mOwner->GetScale() * mTexWidth;
		r.h = mOwner->GetScale() * mTexHeight;
		// ������(����)�� ��ġ�� �߽����� �簢���� ��ġ�Ѵ�
		r.x = static_cast<int>(mOwner->GetPosition().x - r.w / 2);
		r.y = static_cast<int>(mOwner->GetPosition().y - r.h / 2);

		// ��������Ʈ�� �׸���
		SDL_RenderCopyEx(renderer,
			mTexture,	// �׸� �ؽ�ó
			nullptr,	// �׸� �ؽ�ó�� ����. ��ü�� nullptr
			&r,			// �ؽ�ó�� �׸� ȭ���� ����
			-Math::ToDegrees(mOwner->GetRotation()),	// ���ʹ� ������, �� �Լ��� '��' ������ ���Ƿ�, Math.h�� �̿��ؼ� ������ ���� �ٲ��ش�
			nullptr,	// ȸ�� ������. �߾��̸� nullptr
			SDL_FLIP_NONE	// �ø� �ൿ. ���� �� ���� �����
		);
	}

}

void SpriteComponent::SetTexture(SDL_Texture* texture)
{
	mTexture = texture;

	// �ؽ�ó�� ���̿� �ʺ� ���ϱ�. SDL_QueryTexture�� �̿��Ѵ�
	SDL_QueryTexture(texture, nullptr, nullptr, &mTexWidth, &mTexHeight);
}