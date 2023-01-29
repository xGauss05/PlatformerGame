#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene_Menu.h"
#include "Scene_Die.h"
#include "Scene_Level1.h"
#include "EntityManager.h"
#include "Map.h"
#include "Fonts.h"
#include "FadeToBlack.h"
#include "Physics.h"
#include "UserInterface.h"

#include "Defs.h"
#include "Log.h"

Scene_Die::Scene_Die() : Module()
{
	name.Create("scene_die");
}

// Destructor
Scene_Die::~Scene_Die()
{

}

// Called before render is available
bool Scene_Die::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene_Die");
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
	selector_texturePath = config.child("selector").attribute("texturepath").as_string();
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene_Die::Start()
{
	active = false;

	background = app->tex->Load(background_texturePath);
	selector = app->tex->Load(selector_texturePath);
	pointer = app->tex->Load(pointer_texturePath);

	choice = 0;
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

	if (app->entityManager->IsEnabled()) app->entityManager->Disable();
	if (app->ui->IsEnabled()) app->ui->Disable();

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN && !hasSelected)
	{
		if (choice == 2) 
		{
			choice = 0;
		}
		else 
		{
			choice++;
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN && !hasSelected)
	{
		if (choice == 0)
		{
			choice = 2;
		}
		else
		{
			choice--;
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN ||
		app->input->GetMouseButtonDown(SDL_BUTTON_LEFT))
	{

		if (!hasSelected) hasSelected = !hasSelected;

		switch (choice)
		{
		case 0:
			app->ftb->SceneFadeToBlack(this, app->scene, 0);
			app->ui->StartTimer(30000);
			break;
		case 1:
			
			app->entityManager->ReviveAllEntities();
			app->entityManager->NeedsToSpawnAllEntities();
			app->scene->player->ResetGame();
			app->scene->pause = false;
			app->ftb->SceneFadeToBlack(this, app->scene_menu, 45.0f);
			app->scene->pauseBtn->state = GuiControlState::DISABLED;
			app->scene->resumeBtn->state = GuiControlState::DISABLED;
			app->scene->settingsBtn->state = GuiControlState::DISABLED;
			app->scene->backToTitleBtn->state = GuiControlState::DISABLED;
			app->scene->exitBtn->state = GuiControlState::DISABLED;
			break;
		case 2:
			return false;
		default:
			break;
		}
	}

	int x, y;
	app->input->GetMousePosition(x, y);
	if (y >= 575 && y <= 675) choice = 0;

	if (y > 675 && y < 775) choice = 1;

	if (y > 775) choice = 2;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		app->entityManager->ReviveAllEntities();
		app->entityManager->NeedsToSpawnAllEntities();
		app->scene->player->ResetGame();
		app->scene->pause = false;
		app->ftb->SceneFadeToBlack(this, app->scene_menu, 45.0f);
		app->scene->pauseBtn->state = GuiControlState::DISABLED;
		app->scene->resumeBtn->state = GuiControlState::DISABLED;
		app->scene->settingsBtn->state = GuiControlState::DISABLED;
		app->scene->backToTitleBtn->state = GuiControlState::DISABLED;
		app->scene->exitBtn->state = GuiControlState::DISABLED;
	}

	return true;
}

// Called each loop iteration
bool Scene_Die::PostUpdate()
{
	bool ret = true;
	SDL_Rect rect;
	rect.h = 20;
	rect.w = 20;

	app->render->DrawTexture(background, (app->render->camera.x)*-1, app->render->camera.y, NULL);

	int x = (app->render->camera.x)*-1 + 630;
	int y;
	switch (choice)
	{
	case 0:
		y = 575;
		break;
	case 1:
		y = 675;
		break;
	case 2:
		y = 775;
		break;
	default:
		break;
	}

	int a, b;
	app->input->GetMousePosition(a, b);
	rect = currentPointerAnim->GetCurrentFrame();
	currentPointerAnim->Update();
	app->render->DrawTexture(pointer, a, b, &rect);

	app->render->DrawTexture(selector, x, y);
	return ret;
}

// Called before quitting
bool Scene_Die::CleanUp()
{
	LOG("Freeing Scene_Die");
	pointer = nullptr;
	delete pointer;
	selector = nullptr;
	delete selector;
	background = nullptr;
	delete background;

	return true;
}
