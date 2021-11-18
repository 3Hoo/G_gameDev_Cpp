#pragma once

#include "Component.h"
#include <string>
#include <unordered_map>


class AIComponent : public Component
{
public:
	AIComponent(class Actor* owner);

	virtual void Update(float deltaTime) override;

	void ChangeState(const std::string& name);

	// 새 상태를 맵에 추가한다
	void RegisterState(class AIState* state);

private:
	// 상태의 이름과 AIState 인스턴스를 매핑한다
	std::unordered_map<std::string, class AIState*> mStateMap;

	// AI의 현재 상태
	class AIState* mCurrentState;
};

