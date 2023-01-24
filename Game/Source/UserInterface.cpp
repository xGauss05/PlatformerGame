#include "App.h"
#include "UserInterface.h"
#include "Render.h"
#include "Textures.h"
#include "Fonts.h"
#include "Player.h"
#include "Scene_Level1.h"

#include <iostream>
#include <string>
using namespace std;

UserInterface::UserInterface() : Module()
{
	name.Create("UI");
}

UserInterface::~UserInterface() {}

bool UserInterface::Awake(pugi::xml_node&)
{
	this->active = false;
	return true;
}

bool UserInterface::Start()
{
	lives_tex = app->tex->Load("Assets/Textures/lives.png");
	timerOn = false;
	return true;
}

bool UserInterface::PreUpdate()
{
	return true;
}

bool UserInterface::Update(float dt)
{
	/*if (app->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
	{
		StartTimer(30000);
	}*/

	if (timerOn)
	{
		currentTime = high_resolution_clock::now();
		elapsed = duration_cast<milliseconds>(currentTime - startTime);
		milliseconds remaining = timerTest - elapsed;

		string display = std::to_string(remaining.count() * 0.001f);
		display.resize(5);

		app->font->BlitText(1400, 40, 0, display.c_str());

		if (remaining < milliseconds(0))
		{
			app->scene->player->isDead = true;
		}
	}

	return true;
}

bool UserInterface::PostUpdate()
{
	for (int i = 0; i <= app->scene->player->lives; i++)
	{
		int xPos = 1000 + i * 50;
		app->render->DrawTexture(lives_tex, xPos, 20);
	}
	return true;
}

bool UserInterface::CleanUp()
{
	app->tex->UnLoad(lives_tex);
	return true;
}

void UserInterface::StartTimer(int time)
{
	timerOn = true;
	timerTest = milliseconds(time);
	startTime = high_resolution_clock::now();
	currentTime = high_resolution_clock::now();
}
