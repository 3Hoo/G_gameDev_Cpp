#include "MoveComponent.h"
#include "Actor.h"
#include <SDL.h>
#include "PhysWorld.h"

MoveComponent::MoveComponent(Actor* owner, int updateOrder)
	: mAngularSpeed(0)
	, mForwardSpeed(0)
	, mForce(Vector3::Zero)
	, mMass(0)
	, Component(owner)
{
	mVelocity = owner->GetForward();
}

MoveComponent::~MoveComponent()
{

}

float MoveComponent::CalRot(Vector3 force)
{
	Vector3 normalized = force;
	normalized.Normalize();
	Vector3 normalizedForce = mOwner->GetForward();
	normalizedForce.Normalize();

	float newRot = Math::Acos(normalized.Dot(normalized, normalizedForce));

	return newRot;
}

void MoveComponent::Update(float deltaTime)
{
	if (mOwner->GetState() == Actor::EActive)
	{
		if (!Math::NearZero(mAngularSpeed))
		{
			Quaternion rot = mOwner->GetRotation();
			float angle = mAngularSpeed * deltaTime;
			// 회전 증가분을 위한 쿼터니언을 생성
			// 상향축을 기준으로 회전
			Quaternion inc(Vector3::UnitZ, angle);
			// 이전 쿼터니온과 새 쿼터니온을 연결한다
			rot = Quaternion::Concatenate(rot, inc);
			mOwner->SetRotation(rot);
		}

		if (!Math::NearZero(mForwardSpeed) || !Math::NearZero(mStrafeSpeed))
		{
			Vector3 pos = mOwner->GetPosition();
			pos += mOwner->GetForward() * mForwardSpeed * deltaTime;
			// 좌우 이동 속도를 토대로 위치 갱신
			pos += mOwner->GetRight() * mStrafeSpeed * deltaTime;

			mOwner->SetPosition(pos);
		}
		
		if (mOwner->GetJumpState() != Actor::JumpState::EFloor)
		{

		}
	}
	
	/*
	//if (mForce.x != 0.0f || mForce.y != 0.0f)
	{
		Vector2 a = Vector2(mForce.x / mMass, mForce.y / mMass);
		//mOwner->SetRotation(CalRot(mForce));
		mForce = Vector2::Zero;
		mVelocity = mVelocity + (deltaTime * a);
		Vector2 pos = mOwner->GetPosition();
		pos += mVelocity * deltaTime;
		if (pos.x < 0.0f)
		{
			pos.x = 1024.0f;
		}
		else if (pos.x > 1024.0f)
		{
			pos.x = 0.0f;
		}
		if (pos.y < 0.0f)
		{
			pos.y = 768.0f;
		}
		else if (pos.y > 768.0f)
		{
			pos.y = 0.0f;
		}
		mOwner->SetPosition(pos);
		

		//mForces.clear();
		//std::vector<Vector2>().swap(mForces);
		//if (!mForces.empty())
		//{
		//	SDL_Log("There is somthing in mForces");
		//}
	}*/

}