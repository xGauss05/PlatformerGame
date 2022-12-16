#include "Enemy_Walk.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene_Level1.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Map.h"

#include "Fonts.h"

Enemy_Walk::Enemy_Walk() : Entity(EntityType::ENEMY_WALK)
{
	name.Create("enemy_walk");
}

Enemy_Walk::~Enemy_Walk()
{

}

void Enemy_Walk::InitAnims()
{
	rightMove.PushBack({ 9,19,30,18 });
	rightMove.PushBack({ 57,19,30,18 });
	rightMove.PushBack({ 105,19,30,18 });
	rightMove.PushBack({ 153,19,30,18 });
	rightMove.PushBack({ 201,19,30,18 });
	rightMove.PushBack({ 249,19,30,18 });
	rightMove.PushBack({ 297,19,30,18 });
	rightMove.PushBack({ 345,19,30,18 });
	rightMove.speed = 0.1f;
	rightMove.loop = true;

	leftMove.PushBack({ 9,0,30,18 });
	leftMove.PushBack({ 57,0,30,18 });
	leftMove.PushBack({ 105,0,30,18 });
	leftMove.PushBack({ 153,0,30,18 });
	leftMove.PushBack({ 201,0,30,18 });
	leftMove.PushBack({ 249,0,30,18 });
	leftMove.PushBack({ 297,0,30,18 });
	leftMove.PushBack({ 345,0,30,18 });
	leftMove.speed = 0.1f;
	leftMove.loop = true;

	currentAnim = &rightMove;
}

bool Enemy_Walk::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	spawn.x = position.x;
	spawn.y = position.y;
	texturePath = parameters.attribute("texturepath").as_string();
	level = parameters.attribute("level").as_int();

	InitAnims();
	return true;
}

bool Enemy_Walk::Start() {

	//initilize textures
	needsToSpawn = false;
	isDead = false;
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateRectangle(position.x, position.y, 30, 18, DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;
	dieFx = app->audio->LoadFx("Assets/Audio/Fx/enemy_die.wav");
	b2MassData* data = new b2MassData; data->center = b2Vec2((float)40 / 2, (float)88 / 2); data->I = 0.0f; data->mass = 0.1f;
	pbody->body->SetMassData(data);
	delete data;

	return true;
}

void Enemy_Walk::TeleportTo(iPoint position) {
	pbody->body->SetLinearVelocity(b2Vec2(0, 0));
	pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y)), 0.0f);
	pbody->body->ApplyForce(b2Vec2(0.1f, 0.0f), pbody->body->GetWorldCenter(), true);
}

bool Enemy_Walk::Update()
{
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x - (30 / 2));
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y - (18 / 2));
	currentAnim->Update();
	app->render->DrawTexture(texture, position.x, position.y, &(currentAnim->GetCurrentFrame()));

	if (needsToSpawn) {
		TeleportTo(spawn);
		needsToSpawn = false;
	}
	
	if (jumping)
	{
		pbody->body->ApplyForce(b2Vec2(30.0f, -40.0f), pbody->body->GetWorldCenter(), true);
		jumping = false;
	}

	if (pendingToDelete) {
		isDead = true;
		Disable();
	}

	return true;
}

bool Enemy_Walk::CleanUp()
{
	app->tex->UnLoad(texture);

	pendingToDelete = false;
	delete pbody;
	pbody = nullptr;

	return true;
}

void Enemy_Walk::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (physB->ctype == ColliderType::PLAYER)
	{
		if (app->scene->player->dashing == true)
		{
			app->audio->PlayFx(dieFx);
			pendingToDelete = true;
		}
		else
		{
			if (!app->debug->godMode) app->scene->player->isDead = true;
		}
	}

	if (physB->ctype == ColliderType::JUMPTRIGGER)
	{
		jumping = true;
	}
}

void Enemy_Walk::DeathAnimation()
{

}