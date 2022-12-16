#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Textures.h"
#include "Window.h"
#include "Physics.h"
#include "Scene_Menu.h"
#include "Scene_Level1.h"
#include "Scene_Win.h"
#include "Scene_Die.h"
#include "EntityManager.h"
#include "Map.h"
#include "PathFinding.h"
#include "Fonts.h"
#include "FadeToBlack.h"

#include "Defs.h"
#include "Log.h"

Scene_Level1::Scene_Level1() : Module()
{
	name.Create("scene_level1");
}

// Destructor
Scene_Level1::~Scene_Level1()
{

}

// Called before render is available
bool Scene_Level1::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene_Level1");
	bool ret = true;

	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = config.child("player");

	// iterate all objects in the scene
	for (pugi::xml_node enemyNode = config.child("enemy_fly"); enemyNode; enemyNode = enemyNode.next_sibling("enemy_fly"))
	{
		Enemy_Fly* enemy = (Enemy_Fly*)app->entityManager->CreateEntity(EntityType::ENEMY_FLY);
		enemy->parameters = enemyNode;
	}
	for (pugi::xml_node enemyNode = config.child("enemy_walk"); enemyNode; enemyNode = enemyNode.next_sibling("enemy_walk"))
	{
		Enemy_Walk* enemy = (Enemy_Walk*)app->entityManager->CreateEntity(EntityType::ENEMY_WALK);
		enemy->parameters = enemyNode;
	}
	this->active = false;


	for (pugi::xml_node node = config.child("saw").child("pushback"); node; node = node.next_sibling("pushback"))
	{
		sawAnim.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}
	saw_texturePath = config.child("saw").attribute("texturepath").as_string();
	sawAnim.loop = config.child("saw").attribute("loop").as_bool();
	sawAnim.speed = config.child("saw").attribute("animspeed").as_float();
	return ret;
}

// Called before the first frame
bool Scene_Level1::Start()
{
	app->physics->Enable();
	font = app->font->Load("Assets/Textures/font.png", " !ª#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[º]^_`abcdefghijklmnopqrstuvwxyz{|}~             ", 6);

	sawTexture = app->tex->Load(saw_texturePath);

	bool retLoad = app->map->Load();

	// Create walkability map
	if (retLoad) {
		int w, h;
		uchar* data = NULL;

		bool retWalkMap = app->map->CreateWalkabilityMap(w, h, &data);
		if (retWalkMap) app->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);

	}
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
	if (!app->entityManager->IsEnabled()) app->entityManager->Enable();

	if (app->scene_die->IsEnabled()) app->scene_die->Disable();

	if (app->scene_menu->IsEnabled()) app->scene_menu->Disable();

	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) app->LoadGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
	{
		app->audio->PlayFx(player->dieFx);
		app->ftb->SceneFadeToBlack(this, app->scene_die, 40.0f);
	}

	if (app->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN) app->ftb->SceneFadeToBlack(this, app->scene_win, 0);

	// Draw map
	app->map->Draw();

	for (int i = 0; i < saws.Count(); i++)
	{
		sawAnim.Update();
		app->render->DrawTexture(sawTexture,
			METERS_TO_PIXELS(saws.At(i)->data->body->GetPosition().x) - 32,
			METERS_TO_PIXELS(saws.At(i)->data->body->GetPosition().y) - 32,
			&(sawAnim.GetCurrentFrame()));
	}

	if (player->level == 1)
	{
		app->font->BlitText(130, 580, 0, "Use A and D to move");
		app->font->BlitText(540, 630, 0, "Press SPACE to jump");
		app->font->BlitText(1085, 690, 0, "Avoid sharp objects");
	}
	if (player->level == 2)
	{
		app->font->BlitText(1150, 650, 0, "Press SPACE while on");
		app->font->BlitText(1135, 670, 0, "a wall to jump off of it");
		app->font->BlitText(820, 300, 0, "Press SPACE mid-air to do a double jump");
	}
	return true;
}

// Called each loop iteration
bool Scene_Level1::PostUpdate()
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		player->ResetGame();
		app->ftb->SceneFadeToBlack(this, app->scene_menu, 20.0f);
	}

	return ret;
}

// Called before quitting
bool Scene_Level1::CleanUp()
{
	LOG("Freeing Scene_Level1");
	app->tex->UnLoad(sawTexture);
	player = nullptr;
	boxes.Clear();
	saws.Clear();

	app->font->UnLoad(font);

	return true;
}

bool Scene_Level1::LoadState(pugi::xml_node& data)
{
	int x = data.child("player").attribute("x").as_int();
	int y = data.child("player").attribute("y").as_int();

	player->pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y)), 0.0f);
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
