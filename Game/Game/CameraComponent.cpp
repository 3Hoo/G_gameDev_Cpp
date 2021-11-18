#include "CameraComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "AudioSystem.h"

CameraComponent::CameraComponent(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
{

}

void CameraComponent::SetviewMatrix(const Matrix4& view)
{
	// �� ����� �������� ����� �ý��ۿ� �����Ѵ�
	Game* game = mOwner->GetGame();
	game->GetRenderer()->SetViewMatrix(view);
	game->GetAudioSystem()->SetListener(view);
}
