#include "DialogBox.h"
#include "Renderer.h"
#include "Texture.h"
#include "Game.h"

DialogBox::DialogBox(Game* game, const std::string& text, std::function<void()> onOk)
	: UIScreen(game)
{
	// ���̷��α� �ڽ� ��ġ�� ����
	mBGPos = Vector2(0.0f, 0.0f);
	mTitlePos = Vector2(0.0f, 100.0f);
	mNextButtonPos = Vector2(0.0f, 0.0f);

	// ��� �ؽ�ó ����
	mBackGround = mGame->GetRenderer()->GetTexture("Assets/DialogBG.png");
	SetTitle(text, Vector3::Zero, 30);

	// ��ư �߰�
	AddButton("Ok", [onOk]() { onOk(); });
	AddButton("Cancel", [this]() { Close(); });
}

DialogBox::~DialogBox()
{

}