#include "NavComponent.h"
#include "Actor.h"


NavComponent::NavComponent(Actor* owner)
	: MoveComponent(owner)
	, mNextPoint(Vector2::Zero)
{

}

void NavComponent::Update(float deltaTime)
{
	// ���� ������ �����ߴٸ�, �� ������ ��� �ش� ������ ���� ȸ���Ѵ�
	Vector2 diff = mOwner->GetPosition() - mNextPoint;
	
	if (diff.Length() < 2.0f)
	{
		mNextPoint = GetNextPoint();
		TurnTo(mNextPoint);
	}

	// ���͸� ������Ų��
	MoveComponent::Update(deltaTime);
}

Vector2 NavComponent::GetNextPoint()
{
	return Vector2::Zero;
}

void NavComponent::TurnTo(const Vector2& pos)
{
	// ���ο� ���� ���ϴ� ���͸� ���Ѵ�
	Vector2 dir = pos - mOwner->GetPosition();

	// �� ������ �� ���� ������ atan2�̴�
	// �� �� SDLȭ�鿡�� +y�� ȭ�� �Ʒ� �����̹Ƿ�, y���� ������Ų��
	float angle = Math::Atan2(-dir.y, dir.x);
	mOwner->SetRotation(angle);
}