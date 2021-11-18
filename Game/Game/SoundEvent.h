#pragma once
#include <string>
#include "Math.h"

class SoundEvent
{
public:
	SoundEvent();
	
	// ���� FMOD event Instance�� �����ϸ� true ��ȣ��
	bool IsValid();
	
	// �̺�Ʈ�� ó������ �����
	void Restart();

	// �̺�Ʈ ����
	void Stop(bool allowFadeOut = true);

	// Setters
	void SetPaused(bool pause);
	void SetVolume(float value);
	void SetPitch(float value);
	void SetParameter(const std::string& name, float value);

	// Getters
	bool GetPaused() const;
	float GetVolume() const;
	float GetPitch() const;
	float GetParameter(const std::string& name);

	// �̺�Ʈ�� 3D �̸� true�� �����ϴ� �Լ�
	bool Is3D() const;

	// 3D �Ӽ��� �ٷ�� �Լ�. ���� ��ȯ ����� ���ڷ� �޾�, ����� FMOD�� 3D �Ӽ����� ��ȯ�Ѵ�
	void Set3DAttributes(const Matrix4& worldTrans);

protected:
	// �� �����ڴ� protected�� ����, �׸��� AudioSystem�� friend�� ������
	// ��, ���� AudioSystem���� �� �����ڿ� ������ �� ������, �̴� AudioSystem���� SoundEvents�� ID�� �Ҵ��� �� ������ ���Ѵ�
	friend class AudioSystem;
	SoundEvent(class AudioSystem* system, unsigned int id);

private:
	class AudioSystem* mSystem;
	unsigned int mID;
};
