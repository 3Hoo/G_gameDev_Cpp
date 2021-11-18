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
	// ������ ������
	Vector3 mOffset;
	// ī�޶� ���� ����
	Vector3 mUp;
	// ��ġ�� �ʴ� ȸ�� �ӵ�
	float mPitchSpeed;
	// yaw�� �ʴ� ȸ�� �ӵ�
	float mYawSpeed;

};

