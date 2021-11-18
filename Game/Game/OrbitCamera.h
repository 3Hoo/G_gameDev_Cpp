#pragma once
#include "CameraComponent.h"
#include "Math.h"

class OrbitCamera : public CameraComponent
{
public:
	OrbitCamera(class Actor* owner);

	virtual void Update(float deltaTime) override;

	Vector3 GetOffset() const { return mOffset; }
	Vector3 GetUp() const { return mUp; }
	float GetPitchSpeed() const { return mPitchSpeed; }
	float GetYawSpeed() const { return mYawSpeed; }

	void SetOffset(const Vector3 offset) { mOffset = offset; }
	void SetUp(const Vector3 up) { mUp = up; }
	void SetPitchSpeed(const float speed) { mPitchSpeed = speed; }
	void SetYawSpeed(const float speed) { mYawSpeed = speed; }

private:
	// 대상과의 오프셋
	Vector3 mOffset;
	// 카메라 상향 벡터
	Vector3 mUp;
	// 피치의 초당 회전 속도
	float mPitchSpeed;
	// yaw의 초당 회전 속도
	float mYawSpeed;

};

