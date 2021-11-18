#pragma once
#include "CameraComponent.h"
#include "Math.h"

class FollowCamera : public CameraComponent
{
public:
	FollowCamera(class Actor* owner);

	virtual void Update(float deltaTime) override;
	virtual void ProcessInput(const struct InputState& state) override;

	// FollowActor�� ���� �ʱ�ȭ �� �� ȣ���Ѵ�. �̴� ���� ���� �� ī�޶� �ùٸ��� �����ϵ��� �Ѵ�
	void SnapToIdeal();

	float GetPitch() const { return mPitch; }
	float GetPitchSpeed() const { return mPitchSpeed; }
	float GetMaxPitch() const { return mMaxPitch; }
	Vector3 GetOffset() const { return mOffset; }
	Vector3 GetUp() const { return mUp; }
	float GetYawSpeed() const { return mYawSpeed; }

	void SetMaxPitch(float pitch) { mMaxPitch = pitch; }
	void SetOffset(const Vector3 offset) { mOffset = offset; }
	void SetUp(const Vector3 up) { mUp = up; }
	void SetPitchSpeed(const float speed) { mPitchSpeed = speed; }
	void SetYawSpeed(const float speed) { mYawSpeed = speed; }

private:
	// ī�޶��� ��ġ�� ����Ѵ�
	Vector3 ComputeCameraPos() const;

	// ������ ������
	Vector3 mOffset;
	// ī�޶� ���� ����
	Vector3 mUp;
	// pitch�� �ʴ� ȸ�� �ӵ�
	float mPitchSpeed;
	// �������� ���� ���Ϳ��� �־��� �� �ִ� �ִ� pitch �� (�׳� �ö󰡰� �������� �ִ� ����)
	float mMaxPitch;
	// ���� pitch (����)��
	float mPitch;
	//
	float mYawSpeed;

	// ������ ���Ϳ��� ���� �Ÿ�
	float mHorzDist;
	// ������ ���Ϳ��� ���� �Ÿ�
	float mVertDist;
	// ī�޶� ������ �ٶ󺸴� ��� ����(������ ���ͷκ��� ������ ������ �Ÿ�)
	float mTargetDist;
	// ������� ����ϴ� ���� ī�޶��, �⺻ ����� ���� ���� �̻����� ī�޶� �մ� �������� ź��
	float mSpringConstant;

	// ī�޶��� ���� ��ġ(������� ����ϴ� ��ġ)
	Vector3 mActualPos;
	// ī�޶��� �ӵ� (�������� �þ��, ���� ī�޶� �̻����� ī�޶��� ��ġ�� ���ذ��� �ӵ��� �ǹ�)
	Vector3 mVelocity;

	bool mIsMouseRightButton;
};

