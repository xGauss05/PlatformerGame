#include "Scene_Win.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene_Menu.h"
#include "Scene_Level1.h"
#include "Scene_Die.h"
#include "EntityManager.h"
#include "Map.h"
#include "Fonts.h"
#include "FadeToBlack.h"
#include "Physics.h"

#include "Defs.h"
#include "Log.h"



Scene_Win::Scene_Win() : Module()
{
	name.Create("scene_win");
}

// Destructor
Scene_Win::~Scene_Win() {}

// Called before render is available
bool Scene_Win::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene_Win");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene_Win::Start()
{
	active = false;
	return true;
}

// Called each loop iteration
bool Scene_Win::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene_Win::Update(float dt)
{
	app->font->BlitText(700, 400, 0, "You Won!");

	if (app->entityManager->IsEnabled()) {
		app->entityManager->Disable();

		start = high_resolution_clock::now();
	}
	currentTime = high_resolution_clock::now();

	elapsed = duration_cast<milliseconds>(currentTime - start);

	if (elapsed >= milliseconds(5000))
	{
		app->ftb->SceneFadeToBlack(this, app->scene_menu, 25.0f);
	}

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		return false;

	return true;
}

// Called each loop iteration
bool Scene_Win::PostUpdate()
{
	bool ret = true;

	return ret;
}

// Called before quitting
bool Scene_Win::CleanUp()
{
	LOG("Freeing Scene_Menu");
	return true;
}
