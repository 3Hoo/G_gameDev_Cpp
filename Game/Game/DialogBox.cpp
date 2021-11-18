#include "DialogBox.h"
#include "Renderer.h"
#include "Texture.h"
#include "Game.h"

DialogBox::DialogBox(Game* game, const std::string& text, std::function<void()> onOk)
	: UIScreen(game)
{
	// 다이럴로그 박스 위치를 조정
	mBGPos = Vector2(0.0f, 0.0f);
	mTitlePos = Vector2(0.0f, 100.0f);
	mNextButtonPos = Vector2(0.0f, 0.0f);

	// 배경 텍스처 설정
	mBackGround = mGame->GetRenderer()->GetTexture("Assets/DialogBG.png");
	SetTitle(text, Vector3::Zero, 30);

	// 버튼 추가
	AddButton("Ok", [onOk]() { onOk(); });
	AddButton("Cancel", [this]() { Close(); });
}

DialogBox::~DialogBox()
{

}
