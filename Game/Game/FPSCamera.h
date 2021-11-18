#pragma once
#include "CameraComponent.h"
class FPSCamera : public CameraComponent
{
public:
	FPSCamera(class Actor* owner);

	void Update(float deltaTime) override;

	float GetPitch() const { return mPitch; }
	float GetPitchSpeed() const { return mPitchSpeed; }
	float GetMaxPitch() const { return mMaxPitch; }
	float GetYaw() const { return mYaw; }
	float GetYawSpeed() const { return mYawSpeed; }

	void SetPitchSpeed(float speed) { mPitchSpeed = speed; }
	void SetMaxPitch(float pitch) { mMaxPitch = pitch; }
	void SetYawSpeed(float speed) { mYawSpeed = speed; }

private:
	// pitch의 초당 회전 속도
	float mPitchSpeed;
	// yaw의 초당 회전 속도
	float mYawSpeed;
	// 소유자의 전방 벡터에서 멀어질 수 있는 최대 pitch 값 (그냥 올라가고 내려가는 최대 각도)
	float mMaxPitch;
	// 현재 pitch (절대)값
	float mPitch;
	// 현재 yaw 값
	float mYaw;
};

