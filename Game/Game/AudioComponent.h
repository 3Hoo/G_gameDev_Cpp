#pragma once

#include "Component.h"
#include "SoundEvent.h"
#include <string>
#include <vector>

// 사운드 이벤트와 특정 액터를 연관시키기 위한 컴포넌트
// 액터가 움직이면, 관련 이벤트의 3D 속성을 갱신한다
// 액터가 죽으면, 액터와 관련된 사운드 이벤트를 종료한다
class AudioComponent : public Component
{
public:
	AudioComponent(class Actor* owner, int updateOrder = 200);
	~AudioComponent();

	void Update(float deltaTime) override;
	void OnUpdateWorldTransform() override;

	// AudioSystem의 PlayEvent()를 호출하고, 이벤트의 3D 여부를 확인 후 어느 벡터에 SoundEvent를 저장할지 결정.
	// 이벤트가 3D이면 SoundEvent의 Set3DAttributes 함수를 호출
	class SoundEvent PlayEvent(const std::string& name);
	void StopAllEvents();

private:
	std::vector<SoundEvent> mEvents2D;
	std::vector<SoundEvent> mEvents3D;
};

