#include "ExtraLife.h"
#include "Textures.h"
#include "Physics.h"
#include "App.h"
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
	pbody = app->physics->CreateCircle(position.x, position.y, 16, STATIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::EXTRALIFE;
	return true;
}

bool ExtraLife::Update()
{
	return true;
}

bool ExtraLife::CleanUp()
{
	app->tex->UnLoad(texture);
	return true;
}

void ExtraLife::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (physB->ctype == ColliderType::PLAYER)
	{
		app->scene->player->lives++;
		isDead = true;
	}
}

void ExtraLife::DeathAnimation()
{

}
