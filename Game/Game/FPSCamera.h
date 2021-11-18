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
	// pitch�� �ʴ� ȸ�� �ӵ�
	float mPitchSpeed;
	// yaw�� �ʴ� ȸ�� �ӵ�
	float mYawSpeed;
	// �������� ���� ���Ϳ��� �־��� �� �ִ� �ִ� pitch �� (�׳� �ö󰡰� �������� �ִ� ����)
	float mMaxPitch;
	// ���� pitch (����)��
	float mPitch;
	// ���� yaw ��
	float mYaw;
};

