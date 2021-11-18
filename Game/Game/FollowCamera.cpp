#include "FollowCamera.h"
#include "Actor.h"
#include "InputSystem.h"

FollowCamera::FollowCamera(Actor* owner)
	: CameraComponent(owner)
	, mVertDist(150.0f)
	, mHorzDist(350.0f)
	, mTargetDist(100.0f)
	, mSpringConstant(64.0f)
	, mIsMouseRightButton(false)
	, mPitchSpeed(0.0f)
	, mYawSpeed(0.0f)
	, mOffset(Vector3(-350.0f, 150.0f, 0.0f))
	, mUp(Vector3(0.0f, 0.0f, 1.0f))
{

}

static int n = -1;
static int t = 0;

void FollowCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);

	// 스프링 상숫값으로부터 spring dampening 값을 계산
	float dampening = 2.0f * Math::Sqrt(mSpringConstant);

	if (!mIsMouseRightButton)
	{
		// 이상적인 위치 계산
		Vector3 idealPos = ComputeCameraPos();

		// 이상적인 위치와 실제 위치의 차를 계산
		Vector3 diff = mActualPos - idealPos;
		// 스프링의 가속도를 계산한다
		Vector3 acel = -mSpringConstant * diff - dampening * mVelocity;

		// 속도 갱신
		mVelocity += acel * deltaTime;
		// 실제 카메라의 위치 갱신
		mActualPos += mVelocity * deltaTime;

		// 타깃은 소유자 앞에 있는 대상을 의미
		Vector3 target = mOwner->GetPosition() + mOwner->GetForward() * mTargetDist;

		// 뷰 행렬을 생성하기 위해, 실제 위치를 사용한다
		Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, Vector3::UnitZ);
		SetviewMatrix(view);
	}
	else
	{
		// 게임 세계의 상향 벡터와 yaw를 사용한 쿼터니언 생성
		Quaternion yaw(Vector3::UnitZ, mYawSpeed * deltaTime);
		// 오프셋과 상향 벡터를 yaw 쿼터니언을 사용해서 변환
		mOffset = Vector3::Transform(mOffset, yaw);
		mUp = Vector3::Transform(mUp, yaw);

		// 위의 벡터로부터 카메라의 전방/오른축 벡터 계산
		// 전방 벡터 : mOwner.position - (mOwner.position + offset) = -offset
		Vector3 forward = -1.0f * mOffset;
		forward.Normalize();
		// 상향벡터와 전방벡터를 외적해서, 오른축 벡터를 찾아낸다
		Vector3 right = Vector3::Cross(mUp, forward);
		right.Normalize();

		// 카메라 오른축 벡터를 기준으로 회전하는 pitch 쿼터니언 생성
		Quaternion pitch(right, mPitchSpeed * deltaTime);
		// 카메라 오프셋과 상향벡터(카메라의 상향 벡터)를 pitch 쿼터니언으로 회전
		mOffset = Vector3::Transform(mOffset, pitch);
		mUp = Vector3::Transform(mUp, pitch);

		// 변환 행렬을 계산
		Vector3 target = mOwner->GetPosition();
		Vector3 cameraPos = target + mOffset;
		Vector3 idealPos = cameraPos;
		Vector3 diff = mActualPos - idealPos;
		Vector3 acel = -mSpringConstant * diff - dampening * mVelocity;
		mVelocity += acel * deltaTime;
		mActualPos += mVelocity * deltaTime;

		Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, mUp);
		SetviewMatrix(view);
	}

}

void FollowCamera::ProcessInput(const struct InputState& state)
{
	if (state.Mouse.GetButtonState(SDL_BUTTON_RIGHT) == EHeld)
	{
		mIsMouseRightButton = true;
	}
	else
	{
		mIsMouseRightButton = false;
		if (n >= 0)
		{
			n--;
		}
	}
}

void FollowCamera::SnapToIdeal()
{
	// 실제 위치를 이상적인 위치로 설정
	mActualPos = ComputeCameraPos();
	// 속도를 0으로 초기화
	mVelocity = Vector3::Zero;
	// 타깃 지점과 뷰 행렬을 계산
	Vector3 target = mOwner->GetPosition() + mOwner->GetForward() * mTargetDist;
	Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, Vector3::UnitZ);
	SetviewMatrix(view);
}

Vector3 FollowCamera::ComputeCameraPos() const
{
	// 소유자의 뒤쪽 및 위쪽에 카메라 위치 설정
	Vector3 cameraPos = mOwner->GetPosition();
	cameraPos -= mOwner->GetForward() * mHorzDist;
	cameraPos += Vector3::UnitZ * mVertDist;
	return cameraPos;
}