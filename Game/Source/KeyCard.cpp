#include "KeyCard.h"
#include "Textures.h"
#include "Physics.h"
#include "App.h"
#include "Scene_Level1.h"
#include "Point.h"
#include "Input.h"

KeyCard::KeyCard() : Entity(EntityType::KEYCARD)
{
	name.Create("keycard");
}

KeyCard::~KeyCard()
{

}

bool KeyCard::Awake()
{
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	spawn.x = position.x;
	spawn.y = position.y;
	texturePath = parameters.attribute("texturepath").as_string();
	level = parameters.attribute("level").as_int();
	return true;
}

bool KeyCard::Start()
{
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateRectangleSensor(position.x+15, position.y+15, 40, 35, STATIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::KEYCARD;

	return true;
}

bool KeyCard::Update()
{
	app->render->DrawTexture(texture, position.x, position.y);
	if (pendingToDelete)
	{
		isDead = true;
		Disable();
	}
	return true;
}

bool KeyCard::CleanUp()
{
	app->tex->UnLoad(texture);

	pendingToDelete = false;
	delete pbody;
	pbody = nullptr;

	return true;
}

void KeyCard::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (physB->ctype == ColliderType::PLAYER)
	{
		app->scene->player->hasKeyCard = true;
		pendingToDelete = true;
	}
}

void KeyCard::DeathAnimation()
{

}
