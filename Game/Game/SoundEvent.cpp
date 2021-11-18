#include "SoundEvent.h"
#include "AudioSystem.h"
#include <fmod_studio.hpp>

SoundEvent::SoundEvent()
	: mID(0)
	, mSystem(nullptr)
{

}

bool SoundEvent::IsValid()
{
	return (mSystem && mSystem->GetEventInstance(mID) != nullptr);
}

void SoundEvent::Restart()
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;

	if (event)
	{
		event->start();
	}
}

void SoundEvent::Stop(bool allowFadeOut)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;

	if (event)
	{
		FMOD_STUDIO_STOP_MODE mode = allowFadeOut ? 
			FMOD_STUDIO_STOP_ALLOWFADEOUT : FMOD_STUDIO_STOP_IMMEDIATE;
		event->stop(mode);
	}
}

void SoundEvent::SetPaused(bool pause)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;

	if (event)
	{
		event->setPaused(pause);
	}
}
void SoundEvent::SetVolume(float value)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;

	if (event)
	{
		event->setVolume(value);
	}
}
void SoundEvent::SetPitch(float value)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;

	if (event)
	{
		event->setPitch(value);
	}
}
void SoundEvent::SetParameter(const std::string& name, float value)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;

	if (event)
	{
		event->setParameterByName(name.c_str(), value);
	}
}

bool SoundEvent::GetPaused() const
{
	bool paused = false;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;

	if (event)
	{
		event->getPaused(&paused);
	}

	return paused;
}
float SoundEvent::GetVolume() const
{
	float volume = 0.0f; 
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;

	if (event)
	{
		event->getVolume(&volume);
	}

	return volume;
}
float SoundEvent::GetPitch() const
{
	float pitch = 0.0f;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;

	if (event)
	{
		event->getPitch(&pitch);
	}

	return pitch;
}
float SoundEvent::GetParameter(const std::string& name)
{
	float param = 0.0f;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;

	if (event)
	{
		event->getParameterByName(name.c_str(), &param);
	}

	return param;
}

SoundEvent::SoundEvent(AudioSystem* system, unsigned int id)
	: mSystem(system)
	, mID(id)
{
}


bool SoundEvent::Is3D() const
{
	bool retVal = false;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;

	if (event)
	{
		// event description을 얻는다
		FMOD::Studio::EventDescription* ed = nullptr;
		event->getDescription(&ed);

		if (ed)
		{
			ed->is3D(&retVal);	// 이 이벤트는 3D 인가?
		}
	}

	return retVal;
}

namespace
{
	FMOD_VECTOR VecToFMOD(const Vector3& in)
	{
		// Convert from our coordinates (+x forward, +y right, +z up)
		// to FMOD (+z forward, +x right, +y up)
		FMOD_VECTOR v;
		v.x = in.y;
		v.y = in.z;
		v.z = in.x;
		return v;
	}
}

void SoundEvent::Set3DAttributes(const Matrix4& worldTrans)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;

	if (event)
	{
		FMOD_3D_ATTRIBUTES attr;

		// 위치, 전방, 상향 벡터 설정
		attr.position = VecToFMOD(worldTrans.GetTranslation());
		// 세계 공간에서 첫 행은 전방 벡터
		attr.forward = VecToFMOD(worldTrans.GetXAxis());
		// 세계 공간에서 셋째 행은 위쪽 벡터
		attr.up = VecToFMOD(worldTrans.GetZAxis());
		// 속도를 0으로 설정 (도플러 사용시 수정)
		attr.velocity = { 0.0f, 0.0f, 0.0f };

		event->set3DAttributes(&attr);
	}
}
