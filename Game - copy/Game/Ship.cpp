#include "Ship.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "CircleComponent.h"
#include "Laser.h"
#include "Asteroid.h"

Ship::Ship(Game* game)
	: Actor(game)
	, mRightSpeed(0)
	, mDownSpeed(0)
	, asc(nullptr)
	, mLaserCooldown(0.0f)
	, mRespawnTime(2.0f)
	, mRespawn(false)
{
	SpriteComponent* sc = new SpriteComponent(this, 150);
	sc->SetTexture(game->GetTexture("Assets/Ship.png"));

	InputComponent* ic = new InputComponent(this);
	ic->SetForwardKey(SDL_SCANCODE_W);
	ic->SetBackKey(SDL_SCANCODE_S);
	ic->SetClockwiseKey(SDL_SCANCODE_A);
	ic->SetCounterClockwiseKey(SDL_SCANCODE_D);
	ic->SetMaxForwardSpeed(300.0f);
	ic->SetMaxAngularSpeed(Math::TwoPi);
	//ic->SetMaxAngularSpeed(150.0f);
	ic->SetMass(100.0f);

	mCircle = new CircleComponent(this);
	mCircle->SetRadius(35.0f);
	/*
	asc = new AnimSpriteComponent(this);
	std::vector<SDL_Texture*> anims = {
		GetGame()->GetTexture("Assets/Ship01.png"),
		GetGame()->GetTexture("Assets/Ship02.png"),
		GetGame()->GetTexture("Assets/Ship03.png"),
		GetGame()->GetTexture("Assets/Ship04.png")
	};
	asc->SetAnimTextures(anims);
	std::vector<SDL_Texture*> anim_walk = {
		GetGame()->GetTexture("Assets/Character01.png"),
		GetGame()->GetTexture("Assets/Character02.png"),
		GetGame()->GetTexture("Assets/Character03.png"),
		GetGame()->GetTexture("Assets/Character04.png"),
		GetGame()->GetTexture("Assets/Character05.png"),
		GetGame()->GetTexture("Assets/Character06.png"),
	};
	std::vector<SDL_Texture*> anim_jump = {
		GetGame()->GetTexture("Assets/Character07.png"),
		GetGame()->GetTexture("Assets/Character08.png"),
		GetGame()->GetTexture("Assets/Character09.png"),
		GetGame()->GetTexture("Assets/Character10.png"),
		GetGame()->GetTexture("Assets/Character11.png"),
		GetGame()->GetTexture("Assets/Character12.png"),
		GetGame()->GetTexture("Assets/Character13.png"),
		GetGame()->GetTexture("Assets/Character14.png"),
		GetGame()->GetTexture("Assets/Character15.png"),
	};
	//asc->SetAnimTextures(anim_walk, true);
	//asc->SetAnimTextures(anim_jump, false);
	tmc = new TileMapComponent(this);
	tmc->ReadCSV("Assets/MapLayer1.csv");
	SDL_Texture* tilemap = GetGame()->GetTexture("Assets/Tiles.png");
	tmc->SetTexture(tilemap);*/
}

void Ship::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	for (auto ast : GetGame()->GetAsteroids())
	{
		if (Intersect(*mCircle, *(ast->GetCircle())))
		{
			SetPosition(Vector2(1024.0f, 768.0f));
			SetState(EXInput);
			mRespawn = true;
			ast->SetState(EDead);
			break;
		}
	}

	mLaserCooldown -= deltaTime;
	if (mRespawn == true)
	{
		SDL_Log("Ship is dead...\n");
		mRespawnTime -= deltaTime;
		if (mRespawnTime <= 0.0f)
		{
			SDL_Log("Respawn Reset!");
			mRespawn = false;
			mRespawnTime = 1.5f;
			SetPosition(Vector2(512.0f, 384.0f));
			SetState(EActive);
		}
	}
}

void Ship::ProcessKeyboard(const uint8_t* state)
{
	mRightSpeed = 0.0f;
	mDownSpeed = 0.0f;
}

void Ship::ActorInput(const uint8_t* keyState)
{
	if (keyState[SDL_SCANCODE_SPACE] && mLaserCooldown <= 0.0f)
	{
		Laser* laser = new Laser(GetGame());
		laser->SetPosition(GetPosition());
		laser->SetRotation(GetRotation());

		mLaserCooldown = 0.5f;
	}
}
