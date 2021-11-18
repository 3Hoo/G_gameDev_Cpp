#pragma once
#include <unordered_map>
#include <string>
#include "SoundEvent.h"
#include "Math.h"

// .h에 FMOD header를 include 하는 것을 방지하기 위한 전방선언
namespace FMOD
{
	class System;
	namespace Studio
	{
		class System;
		class EventDescription;
		class EventInstance;
		class Bus;
		class VCA;
		class Bank;
		class CommandReplay;
	};
};

class AudioSystem
{
public:
	AudioSystem(class Game* game);
	~AudioSystem();

	bool Initialize();
	void ShutDown();
	void Update(float deltaTime);

	// bank를 로딩, 샘플 데이터를 로드, 이벤트와 관련된 부분을 접근하기 위한 함수
	void LoadBank(const std::string& name);
	void UnloadBank(const std::string& name);
	void UnloadAllBank();

	// 주어진 FMOD EventDescription으로 createInstance로 FMOD EventInstance를 생성, 해당 이벤트 인스턴스의 정수ID를 사용하여 접근하는 SoundEvent를 반환
	class SoundEvent PlayEvent(const std::string& name);


	// 뷰 행렬을 받아, 리스너의 위치/전방벡터/상향벡터 를 구해서 FMOD에 전달
	// 뷰 행렬을 인자로 받으므로, Renderer에서 뷰 행렬을 설정하는 부분에서 이 함수를 호출하면 된다
	// 여기서 뷰 행렬은 "세계공간->뷰 공간" 으로 변환하는 행렬이고, 우리가 원하는 것은 카메라의 세계 공간상의 정보
	void SetListener(const Matrix4& viewMatrix);

	// Bus Controller
	float GetBusVolume(const std::string& name) const;
	bool GetBusPaused(const std::string& name) const;
	void SetBusVolume(const std::string& name, float volume);
	void SetBusPaused(const std::string& name, bool pause);

protected:
	// ID를 받아, 맵에 존재하면 해당 EventInstance 포인터를 반환. 여러 클래스가 이벤트 인스턴스에 접근하는 것을 방지하기 위해 protected
	// 단, SoundEvent는 이 함수로의 접근이 필요하므로 friend로 선언한다
	friend class SoundEvent;
	FMOD::Studio::EventInstance* GetEventInstance(unsigned int id);

private:
	class Game* mGame;

	// FMOD 스튜디오 시스템
	FMOD::Studio::System* mSystem;

	// FMOD 저수준 시스템(거의 접근할 일 없을 것)
	FMOD::System* mLowLevelSystem;

	// 로드된 bank를 관리하는 맵
	std::unordered_map<std::string, FMOD::Studio::Bank*> mBanks;	// <bank의 파일 이름, bank포인터>

	// 이벤트 이름과 EventDescription 맵
	std::unordered_map<std::string, FMOD::Studio::EventDescription*> mEvents;	// <이벤트 경로(event:/Explosion2D), eventdescription *>

	// 모든 인스턴스를 관리하기 위해, <부호x 정수형, 이벤트 인스턴스> 맵을 만든다
	std::unordered_map<unsigned int, FMOD::Studio::EventInstance*> mEventInstances;
	// 이벤트 인스턴스 ID 관리를 위한 정적변수
	static unsigned int sNextID;

	// Bus = 사운드 그룹화
	std::unordered_map<std::string, FMOD::Studio::Bus*> mBuses;
};

