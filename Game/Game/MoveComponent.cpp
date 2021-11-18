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
			// ȸ�� �������� ���� ���ʹϾ��� ����
			// �������� �������� ȸ��
			Quaternion inc(Vector3::UnitZ, angle);
			// ���� ���ʹϿ°� �� ���ʹϿ��� �����Ѵ�
			rot = Quaternion::Concatenate(rot, inc);
			mOwner->SetRotation(rot);
		}

		if (!Math::NearZero(mForwardSpeed) || !Math::NearZero(mStrafeSpeed))
		{
			Vector3 pos = mOwner->GetPosition();
			pos += mOwner->GetForward() * mForwardSpeed * deltaTime;
			// �¿� �̵� �ӵ��� ���� ��ġ ����
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