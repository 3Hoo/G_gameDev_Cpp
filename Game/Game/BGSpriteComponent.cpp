#include "BGSpriteComponent.h"
#include "Actor.h"

BGSpriteComponent::BGSpriteComponent(Actor* owner, int drawOrder)
	: SpriteComponent(owner, drawOrder)
	, mScreenSize(Vector2::Zero)
	, mScrollSpeed(0)
{

}

void BGSpriteComponent::Update(float deltaTime)
{
	SpriteComponent::Update(deltaTime);

	for (auto& bg : mBGTextures)
	{
		// x ������ ���� ����
		bg.mOffset.x += deltaTime * mScrollSpeed;
		
		// �� �ؽ�ó�� ������ �������� ����� ������ ���� ������ ��� �ؽ�ó�� ������ ��ġ�� �ʱ�ȭ�Ѵ�
		if (bg.mOffset.x < -mScreenSize.x)
		{
			bg.mOffset.x = (mBGTextures.size() - 1) * mScreenSize.x - 1;
		}
	}
}

void BGSpriteComponent::Draw(SDL_Renderer* renderer)
{
	for (auto& tex : mBGTextures)
	{
		SDL_Rect r;

		r.w = static_cast<int>(mScreenSize.x);
		r.h = static_cast<int>(mScreenSize.y);

		// mOffset�� �������� ��ġ�� �����Ѵ� => ���Ϳ� ����ִ� ������� ȭ�� ���ʺ��� ���ʴ�� �׷�����
		r.x = static_cast<int>(mOwner->GetPosition().x - r.w / 2 + tex.mOffset.x);
		r.y = static_cast<int>(mOwner->GetPosition().y - r.h / 2 + tex.mOffset.y);

		// ��濡 �׸���
		SDL_RenderCopy(renderer,
			tex.mTexture,
			nullptr,
			&r);
	}
}


void BGSpriteComponent::SetBGTextures(const std::vector<SDL_Texture*>& textures)
{
	int count = 0;

	for (auto tex : textures)
	{
		BGTexture temp;
		temp.mTexture = tex;
		// �� �ؽ�ó�� �������� ȭ���� �ʺ� * count �̴�
		// �׷��� ���� ���ȭ���� ���� ���ȭ���� �����ʿ� ��ġ�ϰ� �ȴ�
		// ���⼭�� �� ��� �̹����� �ʺ� ȭ�� �ʺ�� ���ٰ� �����Ѵ�
		temp.mOffset.x = count * mScreenSize.x;
		temp.mOffset.y = 0;
		mBGTextures.emplace_back(temp);
		count++;
	}
}