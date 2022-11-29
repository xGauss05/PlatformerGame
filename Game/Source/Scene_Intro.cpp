#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene_Menu.h"
#include "Scene_Intro.h"
#include "FadeToBlack.h"

#include "Defs.h"
#include "Log.h"

Scene_Intro::Scene_Intro() : Module()
{
	name.Create("scene_intro");
}

// Destructor
Scene_Intro::~Scene_Intro()
{

}

// Called before render is available
bool Scene_Intro::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene_Intro");
	bool ret = true;
	
	background_texturePath = config.child("background").attribute("texturepath").as_string();
	
	return ret;
}

// Called before the first frame
bool Scene_Intro::Start()
{
	background = app->tex->Load(background_texturePath);
	start = high_resolution_clock::now();
	//SDL_ShowCursor(SDL_DISABLE);

	return true;
}

// Called each loop iteration
bool Scene_Intro::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene_Intro::Update(float dt)
{
	currentTime = high_resolution_clock::now();

	elapsed = duration_cast<milliseconds>(currentTime - start);

	if (elapsed >= milliseconds(3000))
	{
		app->ftb->SceneFadeToBlack(this, app->scene_menu, 25.0f);
	}

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) return false;

	return true;
}

// Called each loop iteration
bool Scene_Intro::PostUpdate()
{
	bool ret = true;

	app->render->DrawTexture(background, app->render->camera.x*-1, app->render->camera.y, NULL);

	return ret;
}

// Called before quitting
bool Scene_Intro::CleanUp()
{
	LOG("Freeing Scene_Intro");

	background = nullptr;
	delete background;

	return true;
}
