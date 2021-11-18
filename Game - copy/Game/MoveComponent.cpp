#include "MoveComponent.h"
#include "Actor.h"
#include <SDL.h>

MoveComponent::MoveComponent(Actor* owner, int updateOrder)
	: mAngularSpeed(0)
	, mForwardSpeed(0)
	, mForce(Vector2::Zero)
	, mMass(0)
	, Component(owner)
{
	mVelocity = owner->GetForward();
}

MoveComponent::~MoveComponent()
{

}

float MoveComponent::CalRot(Vector2 force)
{
	Vector2 normalized = force;
	normalized.Normalize();
	Vector2 normalizedForce = mOwner->GetForward();
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
			float rot = mOwner->GetRotation();
			rot += mAngularSpeed * deltaTime;
			mOwner->SetRotation(rot);
		}

		if (!Math::NearZero(mForwardSpeed))
		{
			Vector2 pos = mOwner->GetPosition();
			pos += mOwner->GetForward() * mForwardSpeed * deltaTime;
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