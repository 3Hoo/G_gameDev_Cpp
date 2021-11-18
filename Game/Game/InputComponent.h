#pragma once
#include "MoveComponent.h"
class InputComponent : public MoveComponent
{
public:
	InputComponent(class Actor* owner);

	void ProcessInput(const struct InputState& state) override;

	// private 변수들에 접근하기 위한 getter/setter
	float GetMaxForward() const { return mMaxForwardSpeed; }
	float GetMaxAngular() const { return mMaxAngularSpeed; }

	int GetForwardKey() const { return mForwardKey; }
	int GetBackKey() const { return mBackKey; }
	int GetClockwiseKey() const { return mClockwiseKey; }
	int GetCounterClockwiseKey() const { return mCounterClockwiseKey; }

	void SetMaxForwardSpeed(const float speed) { mMaxForwardSpeed = speed; }
	void SetMaxAngularSpeed(const float speed) { mMaxAngularSpeed = speed; }

	void SetForwardKey(const int key) { mForwardKey = key; }
	void SetBackKey(const int key) { mBackKey = key; }
	void SetClockwiseKey(const int key) { mClockwiseKey = key; }
	void SetCounterClockwiseKey(const int key) { mCounterClockwiseKey = key; }

private:
	// 최대 전방 속도 / 최대 각 속도
	float mMaxForwardSpeed;
	float mMaxAngularSpeed;

	// 전진 및 후진을 위한 키
	int mForwardKey;
	int mBackKey;

	// 각 운동을 위한 키
	int mClockwiseKey;
	int mCounterClockwiseKey;
};

