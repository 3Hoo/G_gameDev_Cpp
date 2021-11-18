#include "FPSCamera.h"
#include "Actor.h"

FPSCamera::FPSCamera(Actor* owner)
	: CameraComponent(owner)
	, mPitchSpeed(0.0f)
	, mYawSpeed(0.0f)
	, mMaxPitch(Math::Pi / 3.0f)
	, mPitch(0.0f)
{

}

void FPSCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);

	// 카메라의 위치는 소유자의 위치
	Vector3 cameraPos = mOwner->GetPosition();

	// pitch는 소유자의 오른축을 중심으로 한 쿼터니언 회전으로 표현한다
	// pitch의 축은 소유자의 yaw(전방)에 의존(전방벡터가 바뀌면, 우측벡터도 바뀐다)하므로 단순한 y축은 아니다

	// pitch 속도로 pitch를 갱신
	mPitch += mPitchSpeed * deltaTime;
	// pitch 값을 [-max, max] 으로 제한
	mPitch = Math::Clamp(mPitch, -mMaxPitch, mMaxPitch);
	// 소유자의 오른 축 벡터에 대한 pitch 회전을 나타내는 쿼터니언으로 표현
	Quaternion q(mOwner->GetRight(), mPitch);
	// pitch 쿼터니언으로 소유자의 전방 벡터를 회전
	Vector3 viewForward = Vector3::Transform(mOwner->GetForward(), q);
	// 전방 시점 100 단위 앞에 타깃 위치 설정
	Vector3 target = cameraPos + viewForward * 100.0f;
	// pitch 쿼터니언으로 상향축도 회전시킨다
	Vector3 up = Vector3::Transform(Vector3::UnitZ, q);

	// look at 행렬 생성, 뷰 행렬로 설정
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	SetviewMatrix(view);
}