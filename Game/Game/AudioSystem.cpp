#include "AudioSystem.h"
#include <fmod_errors.h>
#include <fmod_studio.hpp>
#include <SDL_log.h>
#include <vector>


unsigned int AudioSystem::sNextID = 0;

namespace
{
	// 헬퍼 함수. 게임의 카메라 좌표를, FMOD에서 사용하는 좌표계로 변환해주는 헬퍼 함수
	FMOD_VECTOR VecToFMOD(const Vector3& in)
	{
		// 게임좌표 (+x 전방, +y 우측, +z 상단)을
		// FMOD 좌표 (+z 전방, +x 오른쪽, +y 상단) 으로 변환
		FMOD_VECTOR v;
		v.x = in.y;
		v.y = in.z;
		v.z = in.x;
		return v;
	}
}

AudioSystem::AudioSystem(Game* game)
	: mGame(game)
	, mSystem(nullptr)
	, mLowLevelSystem(nullptr)
{

}

AudioSystem::~AudioSystem()
{

}

bool AudioSystem::Initialize()
{
	// FMOD 초기화에는 몇 단계가 필요하다
	// 먼저 에러에 대한 로깅을 설정하기 위해 Debug_Initialize 함수를 호출한다
	FMOD::Debug_Initialize(
		FMOD_DEBUG_LEVEL_ERROR,	// 에러일 경우에만 로그를 남긴다	=> 첫 파라미터는 로깅 메세지의 로깅 레벨 수준을 제어
		FMOD_DEBUG_MODE_TTY	// stdout으로 로그 출력					=> 두 번째 파라미터는 로그 메세지를 쓸 위치를 지정한다
	);
	// 커스텀 디버그 코드가 있는 게임에서는 FMOD 로그 메세지 출력을 커스텀 콜백 함수에서 처리하도록 선언하는 것도 가능하다
	SDL_Log("TEST # AudioSystem Debug Initialize # AudioSystem.cpp 48");

	// FMOD 스튜디오 시스템의 인스턴스를 생성한다
	FMOD_RESULT result;
	result = FMOD::Studio::System::create(&mSystem);
	if (result != FMOD_OK)
	{
		SDL_Log("Failed to create FMOD System : %s", FMOD_ErrorString(result));	// FMOD함수는 결과값을 반환해서 호출자에게 그 결과를 알려준다
		return false;															// ErrorString함수로 가독성 있는 메시지로 변환한다
	}
	else
	{
		SDL_Log("TEST # FMOD Studio System Instance Created # AudioSystem.cpp 60");
	}


	// 시스템을 생성했으니, FMOD 시스템의 initialize를 호출한다 (FMOD는 initialize로 호출, update로 업데이트, release로 종료를 반드시 해야한다)
	result = mSystem->initialize(
		512,						// 동시에 출력 가능한 사운드의 최대 갯수
		FMOD_STUDIO_INIT_NORMAL,	// FMOD studio의 동작을 조정. 기본 설정 사용
		FMOD_INIT_NORMAL,			// FMOD 저수준 레벨API 동작을 조정. 기본 설정
		nullptr						// 추가적인 드라이버 데이터를 사용하는 경우 사용하는 파라미터. 대부분 그냥 nullptr 로 둔다
	);
	if (result != FMOD_OK)
	{
		SDL_Log("Failed to create FMOD System : %s", FMOD_ErrorString(result));
		return false;															
	}
	else
	{
		SDL_Log("TEST # FMOD Studio System Initialized # AudioSystem.cpp 77");
	}


	// 초기화를 완료한 후, 저수준 시스템 포인터를 얻어서 저장해둔다
	mSystem->getCoreSystem(&mLowLevelSystem);

	// 모든 FMOD Studio Project는 "Master.bank", "Master.strings.bank" 라는 2개의 기본 뱅크 파일을 가진다
	// 런타임 시 두 마스터 bank를 로드하지 않으면, 다른 bank나 event에 엑세스할 수 없다
	// 마스터 뱅크는 항상 존재하므로, 로드하자
	// 마스터 뱅크 로드 (항상 strings.bank 를 먼저 로드) 
	// => FMOD Studio Project에서 작성한 모든 event와 다른 데이터를 읽을 수 있는 이름을 포함한 특별한 bank
	// => 이것을 로드해야 코드에서 이름으로 bank나 event에 접근이 가능함
	LoadBank("Assets/Master Bank.strings.bank");
	LoadBank("Assets/Master Bank.bank");
	SDL_Log("TEST # Loaded Master Bank # AudioSystem.cpp 89");

	return true;
}

void AudioSystem::ShutDown()
{
	UnloadAllBank();

	if (mSystem)
	{
		mSystem->release();
	}
}

void AudioSystem::Update(float deltaTime)
{
	// 프레임마다 getPlayBackState로 각 이벤트 인스턴스의 재생 상태를 점검
	// 정지 상태에 있는 이벤트 인스턴스만을 해제, 맵에서 제거한다 => 이벤트 정지와 역참조의 타이밍 문제 해결
	// 호출자가 event를 계속 유지시키고 싶으면, event를 정지하는 것이 아니라, 일시 중단 상태로 두면 된다

	// 정지된 이벤트를 찾는다
	std::vector<unsigned int> done;
	for (auto& iter : mEventInstances)
	{
		FMOD::Studio::EventInstance* e = iter.second;
		// 이 이벤트의 상태값을 얻는다
		FMOD_STUDIO_PLAYBACK_STATE state;
		e->getPlaybackState(&state);
		
		if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
		{
			// 이벤트를 해제, id를 done 맵에 추가한다
			e->release();
			done.emplace_back(iter.first);
			SDL_Log("TEST # Release EventInstance %d # AudioSystem.cpp 127", iter.first);
		}
	}

	// 완료된 이벤트 인스턴스를 맵에서 제거
	for (auto id : done)
	{
		mEventInstances.erase(id);
	}

	// FMOD 업데이트
	// FMOD는 프레임마다 한 번씩 update 함수 호출을 요구한다. 이 함수는 3D 오디오 계산 갱신과 같은 작업을 수행
	mSystem->update();
}

void AudioSystem::LoadBank(const std::string& name)
{
	// 두 번 로딩되지 않도록 한다
	if (mBanks.find(name) != mBanks.end())
	{
		SDL_Log("Banks loaded twice");
		return;
	}

	// bank 로드
	FMOD::Studio::Bank* bank = nullptr;
	FMOD_RESULT result = mSystem->loadBankFile(
		name.c_str(),	// bank의 파일 이름
		FMOD_STUDIO_LOAD_BANK_NORMAL,	// 일반적인 방식으로 로딩
		&bank	// bank 포인터 저장
	);

	const int maxPathLength = 512;
	if (result == FMOD_OK)
	{
		SDL_Log("TEST # Loaded Bank file by its name # AudioSystem.cpp 161");
		// bank를 맵에 추가
		mBanks.emplace(name, bank);
		// 스트리밍 형식이 아닌 모든 셈플 데이터를 로드
		bank->loadSampleData();
		// bank의 이벤트의 수를 얻는다
		int numEvents = 0;
		bank->getEventCount(&numEvents);
		if (numEvents > 0)
		{
			// bank에서 event description을 얻는다
			std::vector<FMOD::Studio::EventDescription*> events(numEvents + 1);
			bank->getEventList(events.data(), numEvents + 1, &numEvents);	// (**array, capacity, *count)
			char eventName[maxPathLength];

			for (int i = 0; i < numEvents; i++)
			{
				FMOD::Studio::EventDescription* e = events[i];
				// event:/Explosion2D 같은 이벤트의 경로를 얻는다
				e->getPath(eventName, maxPathLength, nullptr);	// (*path, size, *retrieved)
				// 이벤트를 맵에 추가
				mEvents.emplace(eventName, e);
			}
		}

		// 이 bank 안에 있는 Bus 로드
		int numBuses = 0;
		bank->getBusCount(&numBuses);
		if (numBuses > 0)
		{
			// bank에서 buses 를 얻는다
			std::vector<FMOD::Studio::Bus*> buses(numBuses + 1);
			bank->getBusList(buses.data(), numBuses + 1, &numBuses);
			char busName[maxPathLength];

			for (int i = 0; i < numBuses; i++)
			{
				FMOD::Studio::Bus* b = buses[i];
				// bus:/. 같은 버스의 경로를 얻는다
				b->getPath(busName, maxPathLength, nullptr);
				// 버스를 맵에 추가
				mBuses.emplace(busName, b);
			}
		}

		SDL_Log("TEST # Loaded Bank & Bus # AudioSystem.cpp 206");
	}
	else
	{
		SDL_Log("Failed to load Master Bank by its name");
	}


}

void AudioSystem::UnloadBank(const std::string& name)
{
	auto iter = mBanks.find(name);
	if (iter != mBanks.end())
	{
		// 먼저, 뱅크의 모든 이벤트를 지운다
		const int maxPathLength = 512;
		int numEvents = 0;
		iter->second->getEventCount(&numEvents);
		if (numEvents > 0)
		{
			// bank에서 event description을 얻는다
			std::vector<FMOD::Studio::EventDescription*> events(numEvents);
			iter->second->getEventList(events.data(), numEvents, &numEvents);	// (**array, capacity, *count)
			char eventName[maxPathLength];

			for (int i = 0; i < numEvents; i++)
			{
				FMOD::Studio::EventDescription* e = events[i];
				// event:/Explosion2D 같은 이벤트의 경로를 얻는다
				e->getPath(eventName, maxPathLength, nullptr);	// (*path, size, *retrieved)
				// 해당 이벤트를 지운다
				auto eventi = mEvents.find(eventName);
				if (eventi != mEvents.end())
				{
					mEvents.erase(eventName);
				}
			}
		}
		// 이제 이 뱅크의 버스를 지운다
		int numBuses = 0;
		iter->second->getBusCount(&numBuses);
		if (numBuses > 0)
		{
			// Get list of buses in this bank
			std::vector<FMOD::Studio::Bus*> buses(numBuses);
			iter->second->getBusList(buses.data(), numBuses, &numBuses);
			char busName[512];
			for (int i = 0; i < numBuses; i++)
			{
				FMOD::Studio::Bus* bus = buses[i];
				// Get the path of this bus (like bus:/SFX)
				bus->getPath(busName, 512, nullptr);
				// Remove this bus
				auto busi = mBuses.find(busName);
				if (busi != mBuses.end())
				{
					mBuses.erase(busi);
				}
			}
		}

		// bank를 언로드하고 mBanks 맵에서 제거한다
		iter->second->unloadSampleData();
		iter->second->unload();
		mBanks.erase(iter);
	}
	else
	{
		return;
	}
}

void AudioSystem::UnloadAllBank()
{
	for (auto& iter : mBanks)
	{
		// sample data를 unload하고, bank를 unload 한다
		iter.second->unloadSampleData();
		iter.second->unload();
	}
	mBanks.clear();
	// banks가 없다는 것은, event도 없다는 의미
	mEvents.clear();
}

SoundEvent AudioSystem::PlayEvent(const std::string& name)
{
	// 이 함수로부터 직접 EventInstance 포인터를 반환받아 사용하는 대신, 개발자가 정수 ID를 통해 각 활성화된 이벤트 인스턴스에 접근하게 한다
	// 이 활성화된 인스턴스를 다루기 위해 SoundEvent 라는 클래스를 새로 만든다
	// 이 클래스는 정수 ID를 사용해서 이벤트를 참조한다. 
	// 그리고 이제 이 PlayEvent함수는 SoundEvent의 인스턴스를 반환한다
	static int numInstance = 0;
	unsigned int retID = 0;
	auto iter = mEvents.find(name);
	if (iter != mEvents.end())
	{
		// 이벤트 인스턴스 생성
		FMOD::Studio::EventInstance* event = nullptr;
		iter->second->createInstance(&event);
		if (event)
		{
			// 이벤트 인스턴스를 시작한다
			event->start();
			numInstance++;
			SDL_Log("Test # Event Instance %d Start(%s) # AudioSystem.cpp 310",numInstance, &name);
			// 새 ID를 얻어 맵에 추가
			sNextID++;					// unsigned int이므로, 40억회 이상 반복되면 다시 처음부터 반복하게 된다. 염두해 두는게 좋다
			retID = sNextID;
			mEventInstances.emplace(retID, event);
		}
	}
	else
	{
		SDL_Log("Can't find FMOD event : %s", name);
	}
	return SoundEvent(this, retID);
}

void AudioSystem::SetListener(const Matrix4& viewMatrix)
{
	// 뷰 행렬의 역행렬을 구해서, 필요로 하는 세계 공간의 벡터를 얻는다
	Matrix4 invView = viewMatrix;
	invView.Invert();

	FMOD_3D_ATTRIBUTES listener;

	// 위치와 전방 벡터, 상향 벡터 설정
	listener.position = VecToFMOD(invView.GetTranslation());
	// 뷰 행렬의 역행렬에서 세 번째 행은 전방 벡터
	listener.forward = VecToFMOD(invView.GetZAxis());
	// 뷰 행렬의 역행렬에서 두 번째 행은 상향 벡터


	listener.up = VecToFMOD(invView.GetYAxis());
	// 속도를 0으로 설정. (도플러 효과 사용시 수정)
	listener.velocity = { 0.0f, 0.0f, 0.0f };

	// FMOD로 보낸다 (0 = 리스너는 하나)
	mSystem->setListenerAttributes(0, &listener);
}

float AudioSystem::GetBusVolume(const std::string& name) const
{
	float volume;

	auto iter = mBuses.find(name);
	if (iter != mBuses.end())
	{
		iter->second->getVolume(&volume);
	}

	return volume;
}

bool AudioSystem::GetBusPaused(const std::string& name) const
{
	bool pause;

	auto iter = mBuses.find(name);
	if (iter != mBuses.end())
	{
		iter->second->getPaused(&pause);
	}

	return pause;
}

void AudioSystem::SetBusVolume(const std::string& name, float volume)
{
	auto iter = mBuses.find(name);
	if (iter != mBuses.end())
	{
		iter->second->setVolume(volume);
	}
}

void AudioSystem::SetBusPaused(const std::string& name, bool pause)
{
	auto iter = mBuses.find(name);
	if (iter != mBuses.end())
	{
		iter->second->setPaused(pause);
	}
}

FMOD::Studio::EventInstance* AudioSystem::GetEventInstance(unsigned int id)
{
	FMOD::Studio::EventInstance* event = nullptr;
	auto iter = mEventInstances.find(id);
	if (iter != mEventInstances.end())
	{
		return event = iter->second;
	}
	else
		return nullptr;
}




