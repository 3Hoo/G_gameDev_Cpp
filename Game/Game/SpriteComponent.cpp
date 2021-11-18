#include "SpriteComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder)
	: Component(owner)
	, mTexture(nullptr)
	, mDrawOrder(drawOrder)
	, mTexHeight(0)
	, mTexWidth(0)
{
	mOwner->GetGame()->GetRenderer()->AddSprite(this);
}

SpriteComponent::~SpriteComponent()
{
	mOwner->GetGame()->GetRenderer()->RemoveSprite(this);
}

void SpriteComponent::Draw(Shader* shader)
{
	/*
	* ���⼭�� ������ ��ġ�� ȭ����� ���� ��ġ�� ��ġ�Ѵٰ� �����Ѵ�
	* ���� ������ ������ó�� ȭ���� �����̸鼭 �ɸ��Ͱ� �̵��ϴ� ��쿡�� ����� �� ���� ���.
	* �̸� �����Ϸ��� "ī�޶�"�� ������ �ʿ��ϴ� => 9��
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
	}*/

	if (mTexture)
	{
		// �ؽ�ó�� �ʺ�� ���̷� �簢���� ������
		Matrix4 scaleMat = Matrix4::CreateScale(
			static_cast<float>(mTexWidth),
			static_cast<float>(mTexHeight),
			1.0f);
		Matrix4 world = scaleMat * mOwner->GetWorldTransform();

		// ���� ��ȯ ����� ����
		// ���̴����� "��! �� �ؽ�ó�� �� ����ũ�⸦ ����!" ��� �˷��ִ� ��
		shader->SetMatrixUniform("uWorldTransform", world);

		// ���� �ؽ�ó�� ����
		mTexture->SetActive();

		// �簢���� �׸���
		glDrawElements(
			GL_TRIANGLES,		// �׷��� �� ������.������Ƽ���� Ÿ��
			6,					// �ε��� ���ۿ� �ִ� �ε����� ��
			GL_UNSIGNED_INT,	// �� �ε����� Ÿ��
			nullptr				// nullptr
		);
	}

}

void SpriteComponent::SetTexture(Texture* texture)
{
	mTexture = texture;
	mTexWidth = mTexture->GetWidth();
	mTexHeight = mTexture->GetHeight();
}