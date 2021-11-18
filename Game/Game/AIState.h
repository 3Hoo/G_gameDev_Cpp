#pragma once

class AIState
{
public:
	AIState(class AIComponent* owner) : mOwner(owner) {}

	// 각 상태의 구체적인 행동
	virtual void Update(float deltaTime) = 0;
	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;

	// 상태의 이름 얻기
	virtual const char* GetName() const = 0;

protected:
	class AIComponent* mOwner;
};

class AIPatrol : public AIState
{
public:
	AIPatrol(class AIComponent* owner) : AIState(owner) {}

	// 이 상태에 대한 행위를 정의
	virtual void Update(float deltaTime) override;
	virtual void OnEnter() override;
	virtual void OnExit() override;

	virtual const char* GetName() const override { return "Patrol"; }

};

class AIDeath : public AIState
{
public:
	AIDeath(class AIComponent* owner) : AIState(owner) {}

	// 이 상태에 대한 행위를 정의
	virtual void Update(float deltaTime) override;
	virtual void OnEnter() override;
	virtual void OnExit() override;

	virtual const char* GetName() const override { return "Death"; }
};

class AIAttack : public AIState
{
public:
	AIAttack(class AIComponent* owner) : AIState(owner) {}

	// 이 상태에 대한 행위를 정의
	virtual void Update(float deltaTime) override;
	virtual void OnEnter() override;
	virtual void OnExit() override;

	virtual const char* GetName() const override { return "Attack"; }
};
