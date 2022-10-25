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
Scene_Menu::~Scene_Menu() {}

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
	img = app->tex->Load("Assets/Textures/menu.png");
	arrow = app->tex->Load("Assets/Textures/arrow.png");
	choice = 0;
	
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

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN && !hasSelected) {
		if (choice == 1) {
			choice = 0;
		}
		else {
			choice++;
		}
		if (!hasSelected) {
			//app->audio->PlayFx(selectHover);
		}
	}
	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN && !hasSelected) {
		if (choice == 0) {
			choice = 1;
		}
		else {
			choice--;
		}
		if (!hasSelected) {
			//app->audio->PlayFx(selectHover);
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {

		if (!hasSelected) {
			hasSelected = !hasSelected;
			//app->audio->PlayFx(selected);
		}

		switch (choice) {
		case 0:
			app->ftb->SceneFadeToBlack(this, app->scene, 90);
			break;
		case 1:
			return false;
		default:
			break;
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		return false;

	return true;
}

// Called each loop iteration
bool Scene_Menu::PostUpdate()
{
	bool ret = true;

	app->render->DrawTexture(img, 0, 0, NULL);

	
	int x = 400;
	int y;
	switch (choice) {
	case 0:
		y = 400;
		break;
	case 1:
		y = 580;
		break;
	default:
		break;
	}
 	app->render->DrawTexture(arrow, x, y, NULL);
	return ret;
}

// Called before quitting
bool Scene_Menu::CleanUp()
{
	LOG("Freeing Scene_Menu");

	return true;
}
