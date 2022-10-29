#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene_Die.h"
#include "Scene_Level1.h"
#include "EntityManager.h"
#include "Map.h"
#include "Fonts.h"
#include "FadeToBlack.h"
#include "Physics.h"

#include "Defs.h"
#include "Log.h"

Scene_Die::Scene_Die(bool startEnabled) : Module(startEnabled)
{
	name.Create("scene_die");
	anim.PushBack({ 0, 0, 120, 38 });
	anim.PushBack({ 124, 0, 120, 38 });

	anim.speed = 0.01f;
	currentAnim = &anim;

	pointerArrow.PushBack({ 0, 0, 18, 18 });
	pointerArrow.PushBack({ 25, 0, 18, 18 });

	pointerArrow.speed = 1.0f;
	currentPointerAnim = &pointerArrow;

}

// Destructor
Scene_Die::~Scene_Die() {}

// Called before render is available
bool Scene_Die::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene_Die");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene_Die::Start()
{
	img = app->tex->Load("Assets/Textures/die.png");
	arrow = app->tex->Load("Assets/Textures/arrowAnim.png");
	pointer = app->tex->Load("Assets/Textures/pointer.png");
	choice = 0;
	//SDL_ShowCursor(SDL_DISABLE);
	//app->audio->PlayMusic("Assets/Audio/Music/bgm.ogg");

	//font = app->font->Load("Assets/Textures/font.png", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ.@'&-                       ", 8);

	//app->win->SetTitle(title.GetString());

	return true;
}

// Called each loop iteration
bool Scene_Die::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene_Die::Update(float dt)
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
bool Scene_Die::PostUpdate()
{
	bool ret = true;
	SDL_Rect rect;
	rect.h = 20;
	rect.w = 20;

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
bool Scene_Die::CleanUp()
{
	LOG("Freeing Scene_Die");

	return true;
}