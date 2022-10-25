#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene_Menu.h"
#include "Scene_Level1.h"
#include "EntityManager.h"
#include "Map.h"
#include "Fonts.h"
#include "FadeToBlack.h"
#include "Physics.h"

#include "Defs.h"
#include "Log.h"

Scene_Menu::Scene_Menu(bool startEnabled) : Module(startEnabled)
{
	name.Create("scene_menu");
}

// Destructor
Scene_Menu::~Scene_Menu()
{}

// Called before render is available
bool Scene_Menu::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene_Menu");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene_Menu::Start()
{
	app->physics->Disable();
	img = app->tex->Load("Assets/Textures/menu.png");
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	//font = app->font->Load("Assets/Textures/font.png", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ.@'&-                       ", 8);

	//app->win->SetTitle(title.GetString());

	return true;
}

// Called each loop iteration
bool Scene_Menu::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene_Menu::Update(float dt)
{
	app->render->DrawTexture(img, 0, 0, NULL);
	if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
		app->ftb->SceneFadeToBlack(this, app->scene, 0.0f);

	return true;
}

// Called each loop iteration
bool Scene_Menu::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene_Menu::CleanUp()
{
	LOG("Freeing Scene_Menu");

	return true;
}
