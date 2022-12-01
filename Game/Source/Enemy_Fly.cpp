#include "Enemy_Fly.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene_Level1.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

Enemy_Fly::Enemy_Fly() : Entity(EntityType::ENEMY_FLY)
{
	name.Create("enemy");
}

Enemy_Fly::~Enemy_Fly()
{

}

bool Enemy_Fly::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	return true;
}

bool Enemy_Fly::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateCircle(position.x, position.y, 10, DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;

	return true;
}

bool Enemy_Fly::Update()
{
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x - (32 / 2));
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y - (32 / 2));
	app->render->DrawTexture(texture, position.x, position.y);

	return true;
}

bool Enemy_Fly::CleanUp()
{
	app->tex->UnLoad(texture);

	delete pbody;
	pbody = nullptr;

	return true;
}

void Enemy_Fly::OnCollision(PhysBody* physA, PhysBody* physB)
{

}

void Enemy_Fly::DeathAnimation()
{
}