#pragma once
#include <unordered_map>
#include <string>
#include "SoundEvent.h"
#include "Math.h"

// .h�� FMOD header�� include �ϴ� ���� �����ϱ� ���� ���漱��
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

	// bank�� �ε�, ���� �����͸� �ε�, �̺�Ʈ�� ���õ� �κ��� �����ϱ� ���� �Լ�
	void LoadBank(const std::string& name);
	void UnloadBank(const std::string& name);
	void UnloadAllBank();

	// �־��� FMOD EventDescription���� createInstance�� FMOD EventInstance�� ����, �ش� �̺�Ʈ �ν��Ͻ��� ����ID�� ����Ͽ� �����ϴ� SoundEvent�� ��ȯ
	class SoundEvent PlayEvent(const std::string& name);


	// �� ����� �޾�, �������� ��ġ/���溤��/���⺤�� �� ���ؼ� FMOD�� ����
	// �� ����� ���ڷ� �����Ƿ�, Renderer���� �� ����� �����ϴ� �κп��� �� �Լ��� ȣ���ϸ� �ȴ�
	// ���⼭ �� ����� "�������->�� ����" ���� ��ȯ�ϴ� ����̰�, �츮�� ���ϴ� ���� ī�޶��� ���� �������� ����
	void SetListener(const Matrix4& viewMatrix);

	// Bus Controller
	float GetBusVolume(const std::string& name) const;
	bool GetBusPaused(const std::string& name) const;
	void SetBusVolume(const std::string& name, float volume);
	void SetBusPaused(const std::string& name, bool pause);

protected:
	// ID�� �޾�, �ʿ� �����ϸ� �ش� EventInstance �����͸� ��ȯ. ���� Ŭ������ �̺�Ʈ �ν��Ͻ��� �����ϴ� ���� �����ϱ� ���� protected
	// ��, SoundEvent�� �� �Լ����� ������ �ʿ��ϹǷ� friend�� �����Ѵ�
	friend class SoundEvent;
	FMOD::Studio::EventInstance* GetEventInstance(unsigned int id);

private:
	class Game* mGame;

	// FMOD ��Ʃ��� �ý���
	FMOD::Studio::System* mSystem;

	// FMOD ������ �ý���(���� ������ �� ���� ��)
	FMOD::System* mLowLevelSystem;

	// �ε�� bank�� �����ϴ� ��
	std::unordered_map<std::string, FMOD::Studio::Bank*> mBanks;	// <bank�� ���� �̸�, bank������>

	// �̺�Ʈ �̸��� EventDescription ��
	std::unordered_map<std::string, FMOD::Studio::EventDescription*> mEvents;	// <�̺�Ʈ ���(event:/Explosion2D), eventdescription *>

	// ��� �ν��Ͻ��� �����ϱ� ����, <��ȣx ������, �̺�Ʈ �ν��Ͻ�> ���� �����
	std::unordered_map<unsigned int, FMOD::Studio::EventInstance*> mEventInstances;
	// �̺�Ʈ �ν��Ͻ� ID ������ ���� ��������
	static unsigned int sNextID;

	// Bus = ���� �׷�ȭ
	std::unordered_map<std::string, FMOD::Studio::Bus*> mBuses;
};

