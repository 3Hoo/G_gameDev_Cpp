#include "NavComponent.h"
#include "Actor.h"


NavComponent::NavComponent(Actor* owner)
	: MoveComponent(owner) 
	, mNextPoint(Vector3::Zero)
{

}

void NavComponent::Update(float deltaTime)
{
	// 다음 지점에 도착했다면 이동할 새 지점을 얻고 해당 지점을 향해 회전한다
	Vector3 diff = mOwner->GetPosition() - mNextPoint;
	if (diff.Length() <= 2.0f)
	{
		mNextPoint = GetNextPoint();
		TurnTo(mNextPoint);
	}

	// 액터를 회전시킨다
	MoveComponent::Update(deltaTime);
}

Vector3 NavComponent::GetNextPoint()
{
	return Vector3::Zero;
}

void NavComponent::TurnTo(const Vector3& pos)
{
	// 향하려는 점으로의 벡터를 구한다
	Vector3 dir = pos - mOwner->GetPosition();

	// 새 각도는 이 방향 벡터의 atan2다
	// 이 때 y값을 반전시킨다. +y는 화면 아래 방향 이기 때문
	float angle = Math::Atan2(dir.y, dir.x);
	mOwner->SetRotation(angle);
}