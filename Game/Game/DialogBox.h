#pragma once
#include "UIScreen.h"
#include <functional>

class DialogBox : public UIScreen
{
public:
	DialogBox(class Game* game, const std::string& text, std::function<void()> onOk);
	~DialogBox();

private:
};

