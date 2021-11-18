#pragma once
#include "CameraComponent.h"
#include "Math.h"

class FollowCamera : public CameraComponent
{
public:
	FollowCamera(class Actor* owner);

	virtual void Update(float deltaTime) override;
	virtual void ProcessInput(const struct InputState& state) override;

	// FollowActor가 최초 초기화 될 때 호출한다. 이는 게임 시작 시 카메라가 올바르게 시작하도록 한다
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
	// 카메라의 위치를 계산한다
	Vector3 ComputeCameraPos() const;

	// 대상과의 오프셋
	Vector3 mOffset;
	// 카메라 상향 벡터
	Vector3 mUp;
	// pitch의 초당 회전 속도
	float mPitchSpeed;
	// 소유자의 전방 벡터에서 멀어질 수 있는 최대 pitch 값 (그냥 올라가고 내려가는 최대 각도)
	float mMaxPitch;
	// 현재 pitch (절대)값
	float mPitch;
	//
	float mYawSpeed;

	// 소유자 액터와의 수평 거리
	float mHorzDist;
	// 소유자 액터와의 수직 거리
	float mVertDist;
	// 카메라가 실제로 바라보는 대상 지점(소유자 액터로부터 앞으로 떨어진 거리)
	float mTargetDist;
	// 뷰행렬이 사용하는 실제 카메라와, 기본 계산을 통해 얻은 이상적인 카메라를 잇는 스프링의 탄성
	float mSpringConstant;

	// 카메라의 실제 위치(뷰행렬이 사용하는 위치)
	Vector3 mActualPos;
	// 카메라의 속도 (스프링이 늘어나고, 실제 카메라가 이상적인 카메라의 위치로 향해가는 속도를 의미)
	Vector3 mVelocity;

	bool mIsMouseRightButton;
};

