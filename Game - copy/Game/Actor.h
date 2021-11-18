#pragma once

#include <vector>
#include "Math.h"
#include <cstdint>

/*
* 모든 게임 객체는 하나의 어머니 클래스를 상속받는다
* 이런 구조를 Monolithic class로 부르기도 한다
* 
* 하지만 이러한 계층 구조는 가끔식 "다이아몬드 상속" 을 만들기도 한다
* 이는 여러 버전의 가상함수를 상속받을 수 있어서 문제를 초래할 수 있다
*/

/*
* 모놀리식 대신에 많은 게임은 "컴포넌트 기반" 게임 객체 모델을 함께 사용한다
* 게임 객체 클래스는 존재하지만, 게임 객체의 서브클래스(자식)은 없다
* 대신에 게임 객체 클래스는  필요에 따라 기능을 구현한 "컴포넌트 객체"의 "컬렉션"을 갖고 있다
* 
* 예를 들면 수륙양용 자동차 객체 클래스는 "GroundVehicleComponent"와 "WaterVehicleComponent", "DrawComponent", "CollisionComponent" 등을 가지고 있다
* 
*/

class Actor
{
public:
	// 액터의 상태를 추적하는데 사용된다
	enum State
	{
		EActive,	// 상태 갱신
		EPaused,	//
		EXInput,
		EDead		// 게임에게 액터를 제거하라고 통지함
	};
	
	// 생성자 소멸자
	Actor(class Game* game);
	virtual ~Actor();

	// Game에서 호출하는 Update 함수 (가상 함수 X)
	void Update(float deltaTime);	// UpdateComponents() -> UpdateActor() 호출

	// 액터에 부착된 모든 컴포넌트를 업데이트 (가상 함수 X)
	void UpdateComponents(float deltaTime);

	// 특정 액터에 특화된 업데이트 코드 (가상 함수 O)
	virtual void UpdateActor(float deltaTime);

	// 입력을 처리하는 함수 (오버라이딩 불가)
	void ProcessInput(const uint8_t* keyState);
	// 특정 액터를 위한 입력을 처리하는 함수 (가상함수 O)
	virtual void ActorInput(const uint8_t* keyState);

	// Getter Setter
	const Vector2& GetPosition() const { return mPosition; }
	float GetScale() const { return mScale; }
	float GetRotation() const { return mRotation; }
	Vector2 GetForward() const { return Vector2(Math::Cos(mRotation), -Math::Sin(mRotation)); }

	void SetPosition(const Vector2& pos) { mPosition = pos; }
	void SetScale(const float scale) { mScale = scale; }
	void SetRotation(const float rot) { mRotation = rot; }

	State GetState() const { return mState; }
	void SetState(State state) { mState = state; }

	class Game* GetGame() { return mGame; }


	// 컴포넌트 추가 제거
	void AddComponent(class Component* component); // 새 컴포넌트를 추가할 때마다 컴포넌트 백터를 정렬해서 갱신 순서를 지켜준다
	void RemoveComponent(class Component* component);

private:
	// 액터의 상태
	State mState;

	// 변환
	Vector2 mPosition;	// 액터의 중심점
	float mScale;		// 액터의 배율 (100%이면 1.0f)
	float mRotation;	// 회전 각도 (라디안)

	// 이 액터가 보유한 컴포넌트들
	std::vector<class Component*> mComponents;
	class Game* mGame;
};
/*
* 추가 액터 생성(이 액터는 다른 액터를 생성하는 기능 등)을 포함한 몇 가지 이유 때문에 Game 클래스에 접근해야 한다
* 한 가지 방법은 게임 객체를 전역적으로 접근 가능한 클래스 인스턴스인 "Singleton"으로 만드는 것
* 하지만 이 패턴은 클래스에 여러 인스턴스가 필요하다고 판단되면 문제가 발생
* 
* 그래서 "의존성 주입(Dependency Injection)" 접근법을 사용한다
* 여기서는 액터 생성자가 Game 클래스의 포인터를 받아서 접근할 때 사용하도록 한다
*/