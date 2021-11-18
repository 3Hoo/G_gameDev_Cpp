#include "UIScreen.h"
#include "Game.h"
#include "Shader.h"
#include "Texture.h"
#include "Font.h"
#include "Renderer.h"

Button::Button(const std::string& name, Font* font, std::function<void()> onClick, const Vector2& pos, const Vector2& dims)
	: mPosition(pos)
	, mDimensions(dims)
	, mOnClick(onClick)
	, mFont(font)
	, mHighLighted(false)
	, mNameTex(nullptr)
{
	SetName(name);
}

Button::~Button()
{
	if (mNameTex)
	{
		mNameTex->UnLoad();
		delete mNameTex;
	}
}

void Button::SetName(const std::string& name)
{
	mName = name;

	if (mNameTex)
	{
		mNameTex->UnLoad();
		delete mNameTex;
		mNameTex = nullptr;
	}
	mNameTex = mFont->RenderText(mName);
}

bool Button::ContainsPoint(const Vector2& pt) const
{
	bool no = pt.x < (mPosition.x - (mDimensions.x / 2.0f)) ||
		pt.x > (mPosition.x + (mDimensions.x / 2.0f)) ||
		pt.y < (mPosition.y - (mDimensions.y / 2.0f)) ||
		pt.y > (mPosition.y + (mDimensions.y / 2.0f));
	if (!no)
	{
		//SDL_Log("Button Contain : %b", !no);
	}

	return !no;
}

void Button::OnClick()
{
	if (mOnClick)
	{
		mOnClick();
	}
}

UIScreen::UIScreen(Game* game)
	: mGame(game)
	, mTitle(nullptr)
	, mBackGround(nullptr)
	, mTitlePos(0.0f, 300.0f)
	, mBGPos(0.0f, 250.0f)
	, mNextButtonPos(0.0f, 200.0f)
	, mState(EActive)
{
	game->PushUI(this);
	mFont = mGame->GetFont("Assets/Carlito-Regular.ttf");
	mButtonOn = mGame->GetRenderer()->GetTexture("Assets/ButtonYellow.png");
	mButtonOff = mGame->GetRenderer()->GetTexture("Assets/ButtonBlue.png");
}

UIScreen::~UIScreen()
{
	if (mTitle)
	{
		mTitle->UnLoad();
		delete mTitle;
		mTitle = nullptr;
	}

	if (mBackGround)
	{
		mBackGround->UnLoad();
		delete mBackGround;
		mBackGround = nullptr;
	}

	for (auto b : mButtons)
	{
		delete b;
	}
	mButtons.clear();
}


void UIScreen::Update(float deltaTime)
{

}

void UIScreen::Draw(Shader* shader)
{
	if (mBackGround)
	{
		DrawTexture(shader, mBackGround, mBGPos);
	}

	if (mTitle)
	{
		DrawTexture(shader, mTitle, mTitlePos);
	}

	for (auto b : mButtons)
	{
		// ��ư�� ��� �ؽ�ó�� �׸���
		Texture* tex = b->GetHighLighted() ? mButtonOn : mButtonOff;
		DrawTexture(shader, tex, b->GetPosition());
		// ��ư�� �ؽ�Ʈ�� �׸���
		DrawTexture(shader, b->GetNameTex(), b->GetPosition());
	}
}

void UIScreen::ProcessInput(const uint8_t* keys)
{
	// UI�� ��ư�� �ִ°�?
	if (!mButtons.empty())
	{
		// ���콺�� ��ġ�� ��´�
		int x, y;
		SDL_GetMouseState(&x, &y);
		// ȭ�� �߽��� (0,0)�� ��ǥ�� ��ȯ (1024*768�� ����)
		Vector2 mousePos(static_cast<float>(x), static_cast<float>(y));
		mousePos.x -= mGame->GetRenderer()->GetScreenWidth() * 0.5f;
		mousePos.y = mGame->GetRenderer()->GetScreenHeight() * 0.5f - mousePos.y;

		// ���콺�� ��ġ�� ��ư�� ���̶���Ʈ ó��
		for (auto b : mButtons)
		{
			if (b->ContainsPoint(mousePos))
			{
				b->SetHightLighted(true);
				//SDL_Log("Button %d is HighLighted!", b);
			}
			else
			{
				b->SetHightLighted(false);
			}
		}
	}
}

void UIScreen::HandleKeyPress(int key)
{
	switch (key)
	{
	case SDL_BUTTON_LEFT:
		if (!mButtons.empty())
		{
			for (auto b : mButtons)
			{
				if (b->GetHighLighted())
				{
					b->OnClick();
					break;
				}
			}
		}
		break;
	default:
		break;
	}
}

void UIScreen::Close()
{
	mState = EClosing;
}


void UIScreen::SetTitle(const std::string& text, const Vector3& color, int pointSize)
{
	// ������ �ִ� title texture�� �����
	if (mTitle)
	{
		mTitle->UnLoad();
		delete mTitle;
		mTitle = nullptr;
	}
	mTitle = mFont->RenderText(text, color, pointSize);
}

void UIScreen::DrawTexture(Shader* shader, Texture* texture, const Vector2& offset, float scale)
{
	Matrix4 scaleMat = Matrix4::CreateScale(static_cast<float>(texture->GetWidth() * scale),
											static_cast<float>(texture->GetHeight() * scale), 1.0f);

	// Translate to position on screen
	Matrix4 transMat = Matrix4::CreateTranslation(Vector3(offset.x, offset.y, 0.0f));
	// World Transform ����
	Matrix4 world = scaleMat * transMat;
	shader->SetMatrixUniform("uWorldTransform", world);

	// �ֱ� Texture ����
	texture->SetActive();
	// Quad DRaw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void UIScreen::SetRelativeMouseMode(bool relative)
{
	if (relative)
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
		SDL_GetRelativeMouseState(nullptr, nullptr);
	}
	else
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
}

void UIScreen::AddButton(const std::string& name, std::function<void()> onClick)
{
	Vector2 dims(static_cast<float>(mButtonOn->GetWidth()), static_cast<float>(mButtonOn->GetHeight()));

	Button* b = new Button(name, mFont, onClick, mNextButtonPos, dims);
	mButtons.emplace_back(b);

	// ���� ��ư�� ��ġ�� �����Ѵ�
	// ��ư�� ���̰����� ��ġ���� ���ҽ�Ų ��, �е����� ���Ѵ�
	mNextButtonPos.y -= mButtonOff->GetHeight() + 20.0f;
}