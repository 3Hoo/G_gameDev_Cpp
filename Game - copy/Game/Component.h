#pragma once

#include <cstdint>

class Component
{
public:
	// 생성자
	// 업데이트 순서값이 작을수록 컴포넌트는 더 빨리 갱신된다
	Component(class Actor* owner, int updateOrder = 100);

	// 소멸자
	virtual ~Component();

	// 델타 시간으로 이 컴포넌트를 업데이트
	virtual void Update(float deltaTime);
	int GetUpdateOrder() const { return mUpdateOrder; }

	virtual void ProcessInput(const uint8_t* keyState);

protected:
	// 소유자 액터
	class Actor* mOwner;

	// 컴포넌트의 업데이트 순서
	// 만약 카메라 컴포넌트가 움직임 컴포넌트보다 먼저 업데이트 되버리면? 케릭터는 움직이지도 않았는데 카메라가 먼저 움직여버린다
	// 그래서 컴포넌트들의 업데이트 순서가 중요한 것이다
	int mUpdateOrder;
};

