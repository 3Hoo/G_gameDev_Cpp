#pragma once
#include "CameraComponent.h"
#include "Math.h"
#include <vector>

struct Spline
{
	// Spline에 대한 제어점
	// (segment 상 n개의 점이 있다면, n+2개의 점이 필요하다)
	std::vector<Vector3> mControlPoints;

	// startIdx = P1인 스플라인 세그먼트가 주어졌을 때, t 값을 토대로 위치를 계산
	Vector3 Compute(size_t startIdx, float t) const;
	size_t GetNumPoints() const { return mControlPoints.size(); }
};

class SplineCamera : public CameraComponent
{
public:
	SplineCamera(class Actor* owner);

	virtual void Update(float deltaTime) override;

	// 스플라인 카메라 재시작
	void Restart();


	void SetSpeed(const float speed) { mSpeed = speed; }
	void SetPaused(const bool pause) { mPaused = pause; }
	void SetSpline(const Spline& path) { mPath = path; }

	bool GetPaused() const { return mPaused; }

private:
	// 카메라가 따라가는 스플라인 경로
	Spline mPath;

	// 현재 제어점 인덱스 및 t
	size_t mIndex;
	float mT;

	// 초당 t 변화율
	float mSpeed;

	// 경로를 따라 카메라가 이동해야 하는지의 여부
	bool mPaused;
};

