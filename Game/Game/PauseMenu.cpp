#include "PauseMenu.h"
#include "Game.h"
#include "DialogBox.h"

PauseMenu::PauseMenu(Game* game)
	: UIScreen(game)
{
	mGame->SetGameState(Game::GameState::EPaused);
	SetRelativeMouseMode(false);
	SetTitle("PAUSED");

	AddButton("Resume", [this]() { Close(); });
	AddButton("Quit", [this]() { 
		new DialogBox(mGame, "Do you want to quit?", 
			[this]() { 
			mGame->SetGameState(Game::GameState::EQuit); SDL_Log("TEST # Quit Button is Activated! # PauseMenu.cpp 14"); 
		});
		SDL_Log("TEXT # DialogBox is Created! # PauseMenu.cpp 15");
	});
}

PauseMenu::~PauseMenu()
{
	mGame->SetGameState(Game::GameState::EGamePlay);
	SetRelativeMouseMode(true);
}

void PauseMenu::HandleKeyPress(int key)
{
	UIScreen::HandleKeyPress(key);
	if (key == 'p')
	{
		Close();
		SDL_Log("Closed PauseMenu");
	}
}