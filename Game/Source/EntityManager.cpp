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
		if (pEntity != app->scene->player) {
			pEntity->needsToSpawn = true;
		}
	}
}

void EntityManager::ActivateEnemies() {
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
			else { // if they are on the same level
				if (pEntity->active != true) {
					pEntity->Enable(); // if they are not active, enable them
				}
			}
		}
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
		if (pEntity->active == false) continue;
		ret = item->data->Update();
	}

	return ret;
}

bool EntityManager::LoadState(pugi::xml_node& data)
{
	ActivateEnemies();
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;
	pugi::xml_node node = data.child("enemy");
	for (item = entities.start; item != NULL; item = item->next) {
		pEntity = item->data;
		if (pEntity != app->scene->player &&
			pEntity->level == app->scene->player->level) {
			bool isActiveInfo = node.attribute("active").as_bool();
			if (pEntity->active) // entity is active
			{
				if (isActiveInfo)
				{ // information says is active
					pEntity->pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(node.attribute("x").as_int()),
						PIXEL_TO_METERS(node.attribute("y").as_int())),
						0.0f);
					pEntity->pbody->body->SetLinearVelocity(b2Vec2(node.attribute("x_velocity").as_float(),
																	node.attribute("y_velocity").as_float()));
				}
				else
				{ // information says is not active
					pEntity->Disable();
				}
				pEntity->isDead = node.attribute("isDead").as_bool();
			}
			else // entity is not active
			{
				if (isActiveInfo)
				{ // information says is active
					pEntity->Enable();
					iPoint position;
					position.x = node.attribute("x").as_int();
					position.y = node.attribute("y").as_int();
					pEntity->TeleportTo(position);
					pEntity->pbody->body->SetLinearVelocity(b2Vec2(node.attribute("x_velocity").as_float(),
						node.attribute("y_velocity").as_float()));
					pEntity->isDead = node.attribute("isDead").as_bool();
				}
			}
			node = node.next_sibling("enemy");
		}
	}

	return true;
}

bool EntityManager::SaveState(pugi::xml_node& data)
{
	pugi::xml_node node;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;
	for (item = entities.start; item != NULL; item = item->next) {
		pEntity = item->data;
		if (pEntity != app->scene->player &&
			pEntity->level == app->scene->player->level) {
			node = data.append_child("enemy");
			if (!pEntity->isDead) {
				node.append_attribute("x") = METERS_TO_PIXELS(pEntity->pbody->body->GetPosition().x);
				node.append_attribute("y") = METERS_TO_PIXELS(pEntity->pbody->body->GetPosition().y);
				node.append_attribute("x_velocity") = pEntity->pbody->body->GetLinearVelocity().x;
				node.append_attribute("y_velocity") = pEntity->pbody->body->GetLinearVelocity().y;
			}
			node.append_attribute("isDead") = pEntity->isDead;
			node.append_attribute("active") = pEntity->active;
		}
	}

	return true;
}