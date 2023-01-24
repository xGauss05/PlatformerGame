#include "App.h"
#include "UserInterface.h"
#include "Render.h"
#include "Textures.h"
#include "Fonts.h"
#include "Player.h"
#include "Scene_Level1.h"

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
	lives_tex = app->tex->Load("Assets/Textures/lives2.png");
	return true;
}

bool UserInterface::PreUpdate()
{
	return true;
}

bool UserInterface::Update(float dt)
{
	return true;
}

bool UserInterface::PostUpdate()
{
	app->font->BlitText(500, 100, 0, "Hola que tal this is UI");

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
