#pragma once
#include <string>
#include "Math.h"

class SoundEvent
{
public:
	SoundEvent();
	
	// 관련 FMOD event Instance가 존재하면 true 반호나
	bool IsValid();
	
	// 이벤트를 처음부터 재시작
	void Restart();

	// 이벤트 정지
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

	// 이벤트가 3D 이면 true를 리턴하는 함수
	bool Is3D() const;

	// 3D 속성을 다루는 함수. 세계 변환 행렬을 인자로 받아, 행렬을 FMOD의 3D 속성으로 변환한다
	void Set3DAttributes(const Matrix4& worldTrans);

protected:
	// 이 생성자는 protected로 선언, 그리고 AudioSystem을 friend로 설정함
	// 즉, 오직 AudioSystem만이 이 생성자에 접근할 수 있으며, 이는 AudioSystem만이 SoundEvents에 ID를 할당할 수 있음을 뜻한다
	friend class AudioSystem;
	SoundEvent(class AudioSystem* system, unsigned int id);

private:
	class AudioSystem* mSystem;
	unsigned int mID;
};

