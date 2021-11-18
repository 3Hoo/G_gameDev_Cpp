#pragma once
#include "MoveComponent.h"
class InputComponent : public MoveComponent
{
public:
	InputComponent(class Actor* owner);

	void ProcessInput(const struct InputState& state) override;

	// private �����鿡 �����ϱ� ���� getter/setter
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
	// �ִ� ���� �ӵ� / �ִ� �� �ӵ�
	float mMaxForwardSpeed;
	float mMaxAngularSpeed;

	// ���� �� ������ ���� Ű
	int mForwardKey;
	int mBackKey;

	// �� ��� ���� Ű
	int mClockwiseKey;
	int mCounterClockwiseKey;
};

