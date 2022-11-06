#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Physics.h"
#include "Scene_Menu.h"
#include "Scene_Level1.h"
#include "Scene_Die.h"
#include "EntityManager.h"
#include "Map.h"
#include "Fonts.h"
#include "FadeToBlack.h"

#include "Defs.h"
#include "Log.h"

Scene_Level1::Scene_Level1() : Module()
{
	name.Create("scene_level1");
}

// Destructor
Scene_Level1::~Scene_Level1() {}

// Called before render is available
bool Scene_Level1::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene_Level1");
	bool ret = true;

	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = config.child("player");

	// iterate all objects in the scene
	for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}
	this->active = false;

	return ret;
}

// Called before the first frame
bool Scene_Level1::Start()
{
	//img = app->tex->Load("Assets/Textures/test.png");
	//
	app->physics->Enable();
	font = app->font->Load("Assets/Textures/font.png", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ.@'&-                       ", 8);

	app->map->Load();

	

	return true;
}

// Called each loop iteration
bool Scene_Level1::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene_Level1::Update(float dt)
{
	if (!app->entityManager->IsEnabled()) {
		app->entityManager->Enable();
		player->levelSelector();
		player->TeleportTo(player->spawn);
		app->render->camera.x = 0;
		app->render->camera.y = 0;
	}

	if (app->scene_die->IsEnabled()) {
		app->scene_die->Disable();
	}

	if (app->scene_menu->IsEnabled()) {
		app->scene_menu->Disable();
	}

	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN) {
		app->audio->PlayFx(player->dieFx);
		app->ftb->SceneFadeToBlack(this, app->scene_die, 0);
	}

	// Draw map
	app->map->Draw();

	return true;
}

// Called each loop iteration
bool Scene_Level1::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		player->level = 1;
		player->TeleportTo(player->spawn);
		app->render->camera.x = 0;
		app->render->camera.y = 0;
		app->ftb->SceneFadeToBlack(this, app->scene_menu, 0);
	}

	return ret;
}

// Called before quitting
bool Scene_Level1::CleanUp()
{
	LOG("Freeing Scene_Level1");
	
	return true;
}

bool Scene_Level1::LoadState(pugi::xml_node& data)
{
	int x = data.child("player").attribute("x").as_int();
	int y = data.child("player").attribute("y").as_int();

	player->pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(x),PIXEL_TO_METERS(y)),0.0f);
	player->level = data.child("player").attribute("level").as_int();

	return true;
}

// using append_child and append_attribute
bool Scene_Level1::SaveState(pugi::xml_node& data)
{
	pugi::xml_node node = data.append_child("player");

	node.append_attribute("x") = METERS_TO_PIXELS(player->pbody->body->GetPosition().x);
	node.append_attribute("y") = METERS_TO_PIXELS(player->pbody->body->GetPosition().y);

	node.append_attribute("level") = player->level;

	return true;
}