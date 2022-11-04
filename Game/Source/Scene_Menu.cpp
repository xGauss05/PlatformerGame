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

Scene_Menu::Scene_Menu() : Module()
{
	name.Create("scene_menu");
	anim.PushBack({ 0, 0, 120, 38 });
	anim.PushBack({ 124, 0, 120, 38 });

	anim.speed = 0.01f;
	currentAnim = &anim;

}

// Destructor
Scene_Menu::~Scene_Menu() {}

// Called before render is available
bool Scene_Menu::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene_Menu");
	for (pugi::xml_node node = config.child("pointer").child("pushback"); node; node = node.next_sibling("pushback"))
	{
		pointerArrow.PushBack({ node.attribute("x").as_int(),
								node.attribute("y").as_int(),
								node.attribute("width").as_int(),
								node.attribute("height").as_int() });

	}
	pointerArrow.speed = config.child("pointer").attribute("animspeed").as_float();
	currentPointerAnim = &pointerArrow;

	background_texturePath = config.child("background").attribute("texturepath").as_string();
	pointer_texturePath = config.child("pointer").attribute("texturepath").as_string();
	bool ret = true;


	return ret;
}

// Called before the first frame
bool Scene_Menu::Start()
{

	background = app->tex->Load(background_texturePath);
	arrow = app->tex->Load("Assets/Textures/arrowAnim.png");
	pointer = app->tex->Load(pointer_texturePath);
	choice = 0;
	SDL_ShowCursor(SDL_DISABLE);
	//app->audio->PlayMusic("Assets/Audio/Music/bgm.ogg");

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
	if (app->entityManager->IsEnabled()) {
		app->entityManager->Disable();
	}
	if (app->scene->IsEnabled()) {
		app->scene->Disable();
	}
	if (app->scene_die->IsEnabled()) {
		app->scene_die->Disable();
	}

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

	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN ||
		app->input->GetMouseButtonDown(SDL_BUTTON_LEFT)) {

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

	int x, y;
	app->input->GetMousePosition(x, y);
	if (y >= 400 && y <= 580) choice = 0;

	if (y > 580) choice = 1;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		return false;

	return true;
}

// Called each loop iteration
bool Scene_Menu::PostUpdate()
{
	bool ret = true;
	SDL_Rect rect;
	rect.h = 20;
	rect.w = 20;

	app->render->DrawTexture(background, 0, 0, NULL);


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
	int a, b;
	app->input->GetMousePosition(a, b);
	rect = currentPointerAnim->GetCurrentFrame();
	currentPointerAnim->Update();
	app->render->DrawTexture(pointer, a, b, &rect);
	rect = currentAnim->GetCurrentFrame();
	currentAnim->Update();
	app->render->DrawTexture(arrow, x, y, &rect);
	return ret;
}

// Called before quitting
bool Scene_Menu::CleanUp()
{
	LOG("Freeing Scene_Menu");
	pointer = nullptr;
	delete pointer;
	arrow = nullptr;
	delete arrow;
	background = nullptr;
	delete background;
	
	return true;
}
