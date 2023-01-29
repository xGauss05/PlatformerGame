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
#include "Map.h"

#include "Fonts.h"

Enemy_Fly::Enemy_Fly() : Entity(EntityType::ENEMY_FLY)
{
	name.Create("enemy_fly");
}

Enemy_Fly::~Enemy_Fly()
{

}

void Enemy_Fly::InitAnims()
{
	rightMove.PushBack({ 0,0,39,29 });
	rightMove.PushBack({ 48,0,39,29 });
	rightMove.PushBack({ 96,0,39,29 });
	rightMove.PushBack({ 144,0,39,29 });
	rightMove.speed = 0.1f;
	rightMove.loop = true;

	leftMove.PushBack({ 0,30,39,29 });
	leftMove.PushBack({ 48,30,39,29 });
	leftMove.PushBack({ 96,30,39,29 });
	leftMove.PushBack({ 144,30,39,29 });
	leftMove.speed = 0.1f;
	leftMove.loop = true;

	currentAnim = &rightMove;
}

bool Enemy_Fly::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	spawn.x = position.x;
	spawn.y = position.y;
	texturePath = parameters.attribute("texturepath").as_string();
	level = parameters.attribute("level").as_int();

	InitAnims();
	return true;

}

bool Enemy_Fly::Start() {

	// Starting flags
	needsToSpawn = false;
	isDead = false;

	texture = app->tex->Load(texturePath);
	dieFx = app->audio->LoadFx("Assets/Audio/Fx/enemy_die.wav");

	// Physical body
	pbody = app->physics->CreateRectangle(position.x, position.y, 39, 29, DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;
	b2MassData* data = new b2MassData; data->center = b2Vec2((float)40 / 2, (float)88 / 2); data->I = 0.0f; data->mass = 0.1f;
	pbody->body->SetMassData(data);
	delete data;
	pbody->body->SetGravityScale(0.0f);
	
	// Initial position
	TeleportTo(spawn);

	return true;
}

void Enemy_Fly::TeleportTo(iPoint position) 
{
	pbody->body->SetLinearVelocity(b2Vec2(0, 0));
	pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y)), 0.0f);
	pbody->body->ApplyForce(b2Vec2(0.1f, 0.0f), pbody->body->GetWorldCenter(), true);
}

bool Enemy_Fly::Update()
{
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x - (39 / 2));
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y - (29 / 2));
	currentAnim->Update();
	app->render->DrawTexture(texture, position.x, position.y, &(currentAnim->GetCurrentFrame()));

	if (app->scene->pause) { return true; }

	// Pathfinding

	iPoint entityTile = app->map->ScreenToMap(METERS_TO_PIXELS(this->pbody->body->GetPosition().x),
		METERS_TO_PIXELS(this->pbody->body->GetPosition().y));
	iPoint playerTile = app->map->ScreenToMap(METERS_TO_PIXELS(app->scene->player->pbody->body->GetPosition().x),
		METERS_TO_PIXELS(app->scene->player->pbody->body->GetPosition().y));

	app->pathfinding->CreatePath(entityTile, playerTile);
	pathToPlayer.Clear();

	const DynArray<iPoint>* path = app->pathfinding->GetLastPath();
	for (uint i = 0; i < path->Count(); i++)
	{
		pathToPlayer.PushBack(iPoint(path->At(i)->x, path->At(i)->y));
	}

	// Pathfinding debug visuals
	if (app->debug->debug && app->debug->paths)
	{
		for (uint i = 0; i < pathToPlayer.Count(); ++i)
		{
			iPoint pos = app->map->MapToScreen(pathToPlayer.At(i)->x, pathToPlayer.At(i)->y);
			app->render->DrawTexture(app->debug->enemyPathTex, pos.x, pos.y);
		}
	}

	// Movement
	if (pathToPlayer.Count() > 1)
	{
		int dirX = pathToPlayer.At(1)->x - pathToPlayer.At(0)->x;
		int dirY = pathToPlayer.At(1)->y - pathToPlayer.At(0)->y;

		if (dirX > 0)
		{
			if (pbody->body->GetLinearVelocity().x < speedCap)
			{
				if (currentAnim != &rightMove) currentAnim = &rightMove;

				pbody->body->ApplyForce(b2Vec2(2.0f, 0.0f), pbody->body->GetWorldCenter(), true);
			}
		}
		else if (dirX < 0)
		{
			if (pbody->body->GetLinearVelocity().x > -speedCap)
			{
				if (currentAnim != &leftMove) currentAnim = &leftMove;

				pbody->body->ApplyForce(b2Vec2(-2.0f, 0.0f), pbody->body->GetWorldCenter(), true);
			}
		}
		else
		{
			pbody->body->ApplyForce(b2Vec2(-pbody->body->GetLinearVelocity().x * 0.1f, 0.0f), pbody->body->GetWorldCenter(), true);
		}

		if (dirY > 0)
		{
			if (pbody->body->GetLinearVelocity().y < speedCap)
			{
				pbody->body->ApplyForce(b2Vec2(0.0f, 2.0f), pbody->body->GetWorldCenter(), true);
			}
		}
		else if (dirY < 0)
		{
			if (pbody->body->GetLinearVelocity().y > -speedCap)
			{
				pbody->body->ApplyForce(b2Vec2(0.0f, -2.0f), pbody->body->GetWorldCenter(), true);
			}
		}
		else
		{
			pbody->body->ApplyForce(b2Vec2(0.0f, -pbody->body->GetLinearVelocity().y * 0.1f), pbody->body->GetWorldCenter(), true);
		}
	}

	if (needsToSpawn) 
	{
		TeleportTo(spawn);
		needsToSpawn = false;
	}

	if (pendingToDelete)
	{ 
		isDead = true;
		Disable(); 
	}

	return true;
}

bool Enemy_Fly::CleanUp()
{
	app->tex->UnLoad(texture);

	pendingToDelete = false;
	delete pbody;
	pbody = nullptr;

	return true;
}

void Enemy_Fly::OnCollision(PhysBody* physA, PhysBody* physB)
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
}

void Enemy_Fly::DeathAnimation()
{

}
