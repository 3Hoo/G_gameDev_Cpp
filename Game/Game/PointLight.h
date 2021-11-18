#pragma once
#include "Actor.h"
#include "Math.h"

class PointLight : public Actor
{
public:
	PointLight(class Game* game);
	~PointLight();

	Vector3 GetPosition() { return mPosition; }
	void SetPosition(Vector3 pos) { mPosition = pos; }
	Vector3 GetDiffuseColor() { return mPosition; }
	void SetDiffuseColor(Vector3 diffuse) { mDiffuseColor = diffuse; }
	Vector3 GetSpecColor() { return mPosition; }
	void SetSpecColor(Vector3 SpecColor) { mSpecColor = SpecColor; }
	float GetSpecPower() { return mSpecPower; }
	void SetSpecPower(float sPower) { mSpecPower = sPower; }
	void SetRadius(float r) { mRadius = r; }
	float GetRadius() { return mRadius; }

private:
	Vector3 mPosition;
	Vector3 mDiffuseColor;
	Vector3 mSpecColor;
	float mSpecPower;
	float mRadius;

};

