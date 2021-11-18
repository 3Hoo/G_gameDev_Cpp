#include "InputComponent.h"
#include "Actor.h"

InputComponent::InputComponent(Actor* owner)
	: MoveComponent(owner)
	, mForwardKey(0)
	, mBackKey(0)
	, mClockwiseKey(0)
	, mCounterClockwiseKey(0)
{

}

void InputComponent::ProcessInput(const struct InputState& state)
{
	// MoveComponent을 위한 전방 속도 계산
	//float forwardSpeed = 0.0f;
	//std::vector<Vector2> newForce;

	/*
	if (mOwner->GetState() == mOwner->EActive)
	{
		float forwardSpeed = 0.0f;
		//Vector2 newForce = Vector2(0.0f, 0.0f);
		if (keyState[mForwardKey])
		{
			//newForce = (mOwner->GetForward() * Vector2(mMaxForwardSpeed, mMaxForwardSpeed));
			forwardSpeed += mMaxForwardSpeed;
		}
		if (keyState[mBackKey])
		{
			//newForce = -1.0f * (mOwner->GetForward() * Vector2(mMaxForwardSpeed, mMaxForwardSpeed));
			forwardSpeed -= mMaxForwardSpeed;
		}
		SetForwardSpeed(forwardSpeed);
		//std::vector<Vector2> temp;
		//temp.emplace_back(newForce);

		// MovementComponent을 위한 각 속도 계산
		float angularSpeed = 0.0f;
		if (keyState[mClockwiseKey])
		{
			//Vector2 t = Vector2 (mOwner->GetForward().y, mOwner->GetForward().x * -1);
			//newForce = t * Vector2(mMaxAngularSpeed, mMaxAngularSpeed);
			angularSpeed += mMaxAngularSpeed;
		}
		if (keyState[mCounterClockwiseKey])
		{
			//Vector2 t = Vector2(mOwner->GetForward().y * -1, mOwner->GetForward().x);
			//newForce = t * Vector2(mMaxAngularSpeed, mMaxAngularSpeed);
			angularSpeed -= mMaxAngularSpeed;
		}
		//SetForce(Vector2::Zero);
		//AddForce(newForce);
		SetAngularSpeed(angularSpeed);
	}
	*/

}