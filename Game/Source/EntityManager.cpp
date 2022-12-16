#include "EntityManager.h"
#include "Player.h"
#include "Item.h"
#include "Saw.h"
#include "App.h"
#include "Textures.h"
#include "Scene_Level1.h"
#include "Physics.h"

#include "Defs.h"
#include "Log.h"

EntityManager::EntityManager() : Module()
{
	name.Create("entitymanager");
}

// Destructor
EntityManager::~EntityManager()
{

}

// Called before render is available
bool EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Loading Entity Manager");
	bool ret = true;

	//Iterates over the entities and calls the Awake
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Awake();
	}

	return ret;
}

bool EntityManager::Start() {

	bool ret = true;

	//Iterates over the entities and calls Start
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Start();
	}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;
	ListItem<Entity*>* item;
	item = entities.end;

	while (item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	entities.Clear();

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* entity = nullptr;

	switch (type)
	{

	case EntityType::PLAYER:
		entity = new Player();
		break;

	case EntityType::ITEM:
		entity = new Item();
		break;

	case EntityType::SAW:
		entity = new Saw();
		break;

	case EntityType::ENEMY_FLY:
		entity = new Enemy_Fly();
		break;
	case EntityType::ENEMY_WALK:
		entity = new Enemy_Walk();
		break;

	default: break;
	}

	// Created entities are added to the list
	AddEntity(entity);

	return entity;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL; item = item->next)
	{
		if (item->data == entity) {
			item->data->DeathAnimation();
			entities.Del(item);
		}
	}
}

void EntityManager::AddEntity(Entity* entity)
{
	if (entity != nullptr) entities.Add(entity);
}

void EntityManager::ReviveAllEntities() {
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;
	for (item = entities.start; item != NULL; item = item->next) {
		pEntity = item->data;
		pEntity->isDead = false;
	}
}

void EntityManager::TeleportToSpawnAllEntities() {
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;
	for (item = entities.start; item != NULL; item = item->next) {
		pEntity = item->data;
		pEntity->needsToSpawn = true;
	}
}


bool EntityManager::Update(float dt)
{
	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;
		if (pEntity != app->scene->player) {
			if (pEntity->level != app->scene->player->level) { // check if they're on the same level
				if (pEntity->active != false) pEntity->Disable(); // if they are active, disable them
			}
			else { // if they're not on the same level
				if (pEntity->active != true) {
					if (!pEntity->isDead) pEntity->Enable(); // if they are not active, enable them
				}
					
			}
		}

		if (pEntity->active == false) continue;
		ret = item->data->Update();
	}

	return ret;
}
