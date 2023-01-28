#include "ExtraLife.h"
#include "Textures.h"
#include "Physics.h"
#include "App.h"
#include "Audio.h"
#include "Scene_Level1.h"
#include "Point.h"
#include "Input.h"

ExtraLife::ExtraLife() : Entity(EntityType::EXTRALIFE)
{
	name.Create("extralife");
}

ExtraLife::~ExtraLife()
{

}

bool ExtraLife::Awake()
{
	spawn.x = position.x;
	spawn.y = position.y;

	return true;
}

bool ExtraLife::Start()
{
	texture = app->tex->Load("Assets/Textures/lives.png");
	pickUpFx = app->audio->LoadFx("Assets/Audio/Fx/life.wav");
	pbody = app->physics->CreateRectangleSensor(position.x + 16, position.y + 16, 32, 32, STATIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::EXTRALIFE;
	return true;
}

bool ExtraLife::Update()
{
	app->render->DrawTexture(texture,
		METERS_TO_PIXELS(pbody->body->GetPosition().x) - 20,
		METERS_TO_PIXELS(pbody->body->GetPosition().y) - 20);

	if (pendingToDelete)
	{
		isDead = true;
		Disable();
	}

	return true;
}

bool ExtraLife::CleanUp()
{
	app->tex->UnLoad(texture);

	pendingToDelete = false;
	delete pbody;
	pbody = nullptr;

	return true;
}

void ExtraLife::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (physB->ctype == ColliderType::PLAYER)
	{
		app->scene->player->lives++;
		app->audio->PlayFx(pickUpFx);
		pendingToDelete = true;
	}
}

void ExtraLife::DeathAnimation()
{

}
