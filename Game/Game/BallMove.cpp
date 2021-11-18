#include "BallMove.h"
#include "Actor.h"
#include "PhysWorld.h"
#include "Game.h"
#include "TargetActor.h"
#include "BallActor.h"

BallMove::BallMove(Actor* owner, int updateOrder)
	: MoveComponent(owner, updateOrder)
{

}

BallMove::~BallMove()
{

}

void BallMove::Update(float deltaTime)
{
	// 이동 방향으로 선분을 생성한다
	const float segmentLength = 30.0f;
	Vector3 start = mOwner->GetPosition();
	Vector3 dir = mOwner->GetForward();
	Vector3 end = start + dir * segmentLength;
	LineSegment ls(start, end);

	// 선분과 게임 세계와의 테스트 진행
	PhysWorld* phys = mOwner->GetGame()->GetPhysWorld();
	PhysWorld::CollisionInfo info;
	// 충돌한 것이 플레이어의 BoxComponent이면 무시한다
	if (phys->SegmentCast(ls, info) && info.mActor != mPlayer)
	{
		// 충돌했다면, 표면 법선을 기준으로 해서 반사시킨다
		dir = Vector3::Reflect(dir, info.mNormal);
		mOwner->RotateToNewForward(dir);
		// Did we hit a target?
		TargetActor* target = dynamic_cast<TargetActor*>(info.mActor);
		if (target)
		{
			static_cast<BallActor*>(mOwner)->HitTarget();
		}
	}

	// 전진 속도와 델타 시간으로 베이스 클래스 이동을 갱신
	MoveComponent::Update(deltaTime);
}