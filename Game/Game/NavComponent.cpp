#include "NavComponent.h"
#include "Actor.h"


NavComponent::NavComponent(Actor* owner)
	: MoveComponent(owner) 
	, mNextPoint(Vector3::Zero)
{

}

void NavComponent::Update(float deltaTime)
{
	// ���� ������ �����ߴٸ� �̵��� �� ������ ��� �ش� ������ ���� ȸ���Ѵ�
	Vector3 diff = mOwner->GetPosition() - mNextPoint;
	if (diff.Length() <= 2.0f)
	{
		mNextPoint = GetNextPoint();
		TurnTo(mNextPoint);
	}

	// ���͸� ȸ����Ų��
	MoveComponent::Update(deltaTime);
}

Vector3 NavComponent::GetNextPoint()
{
	return Vector3::Zero;
}

void NavComponent::TurnTo(const Vector3& pos)
{
	// ���Ϸ��� �������� ���͸� ���Ѵ�
	Vector3 dir = pos - mOwner->GetPosition();

	// �� ������ �� ���� ������ atan2��
	// �� �� y���� ������Ų��. +y�� ȭ�� �Ʒ� ���� �̱� ����
	float angle = Math::Atan2(dir.y, dir.x);
	mOwner->SetRotation(angle);
}