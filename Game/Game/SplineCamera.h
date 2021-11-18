#pragma once
#include "CameraComponent.h"
#include "Math.h"
#include <vector>

struct Spline
{
	// Spline�� ���� ������
	// (segment �� n���� ���� �ִٸ�, n+2���� ���� �ʿ��ϴ�)
	std::vector<Vector3> mControlPoints;

	// startIdx = P1�� ���ö��� ���׸�Ʈ�� �־����� ��, t ���� ���� ��ġ�� ���
	Vector3 Compute(size_t startIdx, float t) const;
	size_t GetNumPoints() const { return mControlPoints.size(); }
};

class SplineCamera : public CameraComponent
{
public:
	SplineCamera(class Actor* owner);

	virtual void Update(float deltaTime) override;

	// ���ö��� ī�޶� �����
	void Restart();


	void SetSpeed(const float speed) { mSpeed = speed; }
	void SetPaused(const bool pause) { mPaused = pause; }
	void SetSpline(const Spline& path) { mPath = path; }

	bool GetPaused() const { return mPaused; }

private:
	// ī�޶� ���󰡴� ���ö��� ���
	Spline mPath;

	// ���� ������ �ε��� �� t
	size_t mIndex;
	float mT;

	// �ʴ� t ��ȭ��
	float mSpeed;

	// ��θ� ���� ī�޶� �̵��ؾ� �ϴ����� ����
	bool mPaused;
};

