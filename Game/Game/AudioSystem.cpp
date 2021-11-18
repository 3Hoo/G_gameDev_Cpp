#include "AudioSystem.h"
#include <fmod_errors.h>
#include <fmod_studio.hpp>
#include <SDL_log.h>
#include <vector>


unsigned int AudioSystem::sNextID = 0;

namespace
{
	// ���� �Լ�. ������ ī�޶� ��ǥ��, FMOD���� ����ϴ� ��ǥ��� ��ȯ���ִ� ���� �Լ�
	FMOD_VECTOR VecToFMOD(const Vector3& in)
	{
		// ������ǥ (+x ����, +y ����, +z ���)��
		// FMOD ��ǥ (+z ����, +x ������, +y ���) ���� ��ȯ
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
	// FMOD �ʱ�ȭ���� �� �ܰ谡 �ʿ��ϴ�
	// ���� ������ ���� �α��� �����ϱ� ���� Debug_Initialize �Լ��� ȣ���Ѵ�
	FMOD::Debug_Initialize(
		FMOD_DEBUG_LEVEL_ERROR,	// ������ ��쿡�� �α׸� �����	=> ù �Ķ���ʹ� �α� �޼����� �α� ���� ������ ����
		FMOD_DEBUG_MODE_TTY	// stdout���� �α� ���					=> �� ��° �Ķ���ʹ� �α� �޼����� �� ��ġ�� �����Ѵ�
	);
	// Ŀ���� ����� �ڵ尡 �ִ� ���ӿ����� FMOD �α� �޼��� ����� Ŀ���� �ݹ� �Լ����� ó���ϵ��� �����ϴ� �͵� �����ϴ�
	SDL_Log("TEST # AudioSystem Debug Initialize # AudioSystem.cpp 48");

	// FMOD ��Ʃ��� �ý����� �ν��Ͻ��� �����Ѵ�
	FMOD_RESULT result;
	result = FMOD::Studio::System::create(&mSystem);
	if (result != FMOD_OK)
	{
		SDL_Log("Failed to create FMOD System : %s", FMOD_ErrorString(result));	// FMOD�Լ��� ������� ��ȯ�ؼ� ȣ���ڿ��� �� ����� �˷��ش�
		return false;															// ErrorString�Լ��� ������ �ִ� �޽����� ��ȯ�Ѵ�
	}
	else
	{
		SDL_Log("TEST # FMOD Studio System Instance Created # AudioSystem.cpp 60");
	}


	// �ý����� ����������, FMOD �ý����� initialize�� ȣ���Ѵ� (FMOD�� initialize�� ȣ��, update�� ������Ʈ, release�� ���Ḧ �ݵ�� �ؾ��Ѵ�)
	result = mSystem->initialize(
		512,						// ���ÿ� ��� ������ ������ �ִ� ����
		FMOD_STUDIO_INIT_NORMAL,	// FMOD studio�� ������ ����. �⺻ ���� ���
		FMOD_INIT_NORMAL,			// FMOD ������ ����API ������ ����. �⺻ ����
		nullptr						// �߰����� ����̹� �����͸� ����ϴ� ��� ����ϴ� �Ķ����. ��κ� �׳� nullptr �� �д�
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


	// �ʱ�ȭ�� �Ϸ��� ��, ������ �ý��� �����͸� �� �����صд�
	mSystem->getCoreSystem(&mLowLevelSystem);

	// ��� FMOD Studio Project�� "Master.bank", "Master.strings.bank" ��� 2���� �⺻ ��ũ ������ ������
	// ��Ÿ�� �� �� ������ bank�� �ε����� ������, �ٸ� bank�� event�� �������� �� ����
	// ������ ��ũ�� �׻� �����ϹǷ�, �ε�����
	// ������ ��ũ �ε� (�׻� strings.bank �� ���� �ε�) 
	// => FMOD Studio Project���� �ۼ��� ��� event�� �ٸ� �����͸� ���� �� �ִ� �̸��� ������ Ư���� bank
	// => �̰��� �ε��ؾ� �ڵ忡�� �̸����� bank�� event�� ������ ������
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
	// �����Ӹ��� getPlayBackState�� �� �̺�Ʈ �ν��Ͻ��� ��� ���¸� ����
	// ���� ���¿� �ִ� �̺�Ʈ �ν��Ͻ����� ����, �ʿ��� �����Ѵ� => �̺�Ʈ ������ �������� Ÿ�̹� ���� �ذ�
	// ȣ���ڰ� event�� ��� ������Ű�� ������, event�� �����ϴ� ���� �ƴ϶�, �Ͻ� �ߴ� ���·� �θ� �ȴ�

	// ������ �̺�Ʈ�� ã�´�
	std::vector<unsigned int> done;
	for (auto& iter : mEventInstances)
	{
		FMOD::Studio::EventInstance* e = iter.second;
		// �� �̺�Ʈ�� ���°��� ��´�
		FMOD_STUDIO_PLAYBACK_STATE state;
		e->getPlaybackState(&state);
		
		if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
		{
			// �̺�Ʈ�� ����, id�� done �ʿ� �߰��Ѵ�
			e->release();
			done.emplace_back(iter.first);
			SDL_Log("TEST # Release EventInstance %d # AudioSystem.cpp 127", iter.first);
		}
	}

	// �Ϸ�� �̺�Ʈ �ν��Ͻ��� �ʿ��� ����
	for (auto id : done)
	{
		mEventInstances.erase(id);
	}

	// FMOD ������Ʈ
	// FMOD�� �����Ӹ��� �� ���� update �Լ� ȣ���� �䱸�Ѵ�. �� �Լ��� 3D ����� ��� ���Ű� ���� �۾��� ����
	mSystem->update();
}

void AudioSystem::LoadBank(const std::string& name)
{
	// �� �� �ε����� �ʵ��� �Ѵ�
	if (mBanks.find(name) != mBanks.end())
	{
		SDL_Log("Banks loaded twice");
		return;
	}

	// bank �ε�
	FMOD::Studio::Bank* bank = nullptr;
	FMOD_RESULT result = mSystem->loadBankFile(
		name.c_str(),	// bank�� ���� �̸�
		FMOD_STUDIO_LOAD_BANK_NORMAL,	// �Ϲ����� ������� �ε�
		&bank	// bank ������ ����
	);

	const int maxPathLength = 512;
	if (result == FMOD_OK)
	{
		SDL_Log("TEST # Loaded Bank file by its name # AudioSystem.cpp 161");
		// bank�� �ʿ� �߰�
		mBanks.emplace(name, bank);
		// ��Ʈ���� ������ �ƴ� ��� ���� �����͸� �ε�
		bank->loadSampleData();
		// bank�� �̺�Ʈ�� ���� ��´�
		int numEvents = 0;
		bank->getEventCount(&numEvents);
		if (numEvents > 0)
		{
			// bank���� event description�� ��´�
			std::vector<FMOD::Studio::EventDescription*> events(numEvents + 1);
			bank->getEventList(events.data(), numEvents + 1, &numEvents);	// (**array, capacity, *count)
			char eventName[maxPathLength];

			for (int i = 0; i < numEvents; i++)
			{
				FMOD::Studio::EventDescription* e = events[i];
				// event:/Explosion2D ���� �̺�Ʈ�� ��θ� ��´�
				e->getPath(eventName, maxPathLength, nullptr);	// (*path, size, *retrieved)
				// �̺�Ʈ�� �ʿ� �߰�
				mEvents.emplace(eventName, e);
			}
		}

		// �� bank �ȿ� �ִ� Bus �ε�
		int numBuses = 0;
		bank->getBusCount(&numBuses);
		if (numBuses > 0)
		{
			// bank���� buses �� ��´�
			std::vector<FMOD::Studio::Bus*> buses(numBuses + 1);
			bank->getBusList(buses.data(), numBuses + 1, &numBuses);
			char busName[maxPathLength];

			for (int i = 0; i < numBuses; i++)
			{
				FMOD::Studio::Bus* b = buses[i];
				// bus:/. ���� ������ ��θ� ��´�
				b->getPath(busName, maxPathLength, nullptr);
				// ������ �ʿ� �߰�
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
		// ����, ��ũ�� ��� �̺�Ʈ�� �����
		const int maxPathLength = 512;
		int numEvents = 0;
		iter->second->getEventCount(&numEvents);
		if (numEvents > 0)
		{
			// bank���� event description�� ��´�
			std::vector<FMOD::Studio::EventDescription*> events(numEvents);
			iter->second->getEventList(events.data(), numEvents, &numEvents);	// (**array, capacity, *count)
			char eventName[maxPathLength];

			for (int i = 0; i < numEvents; i++)
			{
				FMOD::Studio::EventDescription* e = events[i];
				// event:/Explosion2D ���� �̺�Ʈ�� ��θ� ��´�
				e->getPath(eventName, maxPathLength, nullptr);	// (*path, size, *retrieved)
				// �ش� �̺�Ʈ�� �����
				auto eventi = mEvents.find(eventName);
				if (eventi != mEvents.end())
				{
					mEvents.erase(eventName);
				}
			}
		}
		// ���� �� ��ũ�� ������ �����
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

		// bank�� ��ε��ϰ� mBanks �ʿ��� �����Ѵ�
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
		// sample data�� unload�ϰ�, bank�� unload �Ѵ�
		iter.second->unloadSampleData();
		iter.second->unload();
	}
	mBanks.clear();
	// banks�� ���ٴ� ����, event�� ���ٴ� �ǹ�
	mEvents.clear();
}

SoundEvent AudioSystem::PlayEvent(const std::string& name)
{
	// �� �Լ��κ��� ���� EventInstance �����͸� ��ȯ�޾� ����ϴ� ���, �����ڰ� ���� ID�� ���� �� Ȱ��ȭ�� �̺�Ʈ �ν��Ͻ��� �����ϰ� �Ѵ�
	// �� Ȱ��ȭ�� �ν��Ͻ��� �ٷ�� ���� SoundEvent ��� Ŭ������ ���� �����
	// �� Ŭ������ ���� ID�� ����ؼ� �̺�Ʈ�� �����Ѵ�. 
	// �׸��� ���� �� PlayEvent�Լ��� SoundEvent�� �ν��Ͻ��� ��ȯ�Ѵ�
	static int numInstance = 0;
	unsigned int retID = 0;
	auto iter = mEvents.find(name);
	if (iter != mEvents.end())
	{
		// �̺�Ʈ �ν��Ͻ� ����
		FMOD::Studio::EventInstance* event = nullptr;
		iter->second->createInstance(&event);
		if (event)
		{
			// �̺�Ʈ �ν��Ͻ��� �����Ѵ�
			event->start();
			numInstance++;
			SDL_Log("Test # Event Instance %d Start(%s) # AudioSystem.cpp 310",numInstance, &name);
			// �� ID�� ��� �ʿ� �߰�
			sNextID++;					// unsigned int�̹Ƿ�, 40��ȸ �̻� �ݺ��Ǹ� �ٽ� ó������ �ݺ��ϰ� �ȴ�. ������ �δ°� ����
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
	// �� ����� ������� ���ؼ�, �ʿ�� �ϴ� ���� ������ ���͸� ��´�
	Matrix4 invView = viewMatrix;
	invView.Invert();

	FMOD_3D_ATTRIBUTES listener;

	// ��ġ�� ���� ����, ���� ���� ����
	listener.position = VecToFMOD(invView.GetTranslation());
	// �� ����� ����Ŀ��� �� ��° ���� ���� ����
	listener.forward = VecToFMOD(invView.GetZAxis());
	// �� ����� ����Ŀ��� �� ��° ���� ���� ����


	listener.up = VecToFMOD(invView.GetYAxis());
	// �ӵ��� 0���� ����. (���÷� ȿ�� ���� ����)
	listener.velocity = { 0.0f, 0.0f, 0.0f };

	// FMOD�� ������ (0 = �����ʴ� �ϳ�)
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




