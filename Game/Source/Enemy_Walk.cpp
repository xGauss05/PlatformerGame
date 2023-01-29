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

	// Starting flags
	needsToSpawn = false;
	isDead = false;

	texture = app->tex->Load(texturePath);
	dieFx = app->audio->LoadFx("Assets/Audio/Fx/enemy_die.wav");

	// Physical body
	lastImpulse = iPoint(0, 0);
	pbody = app->physics->CreateRectangle(position.x, position.y, 30, 18, DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;
	b2MassData* data = new b2MassData; data->center = b2Vec2((float)40 / 2, (float)88 / 2); data->I = 0.0f; data->mass = 0.1f;
	pbody->body->SetMassData(data);
	delete data;

	// Initial position
	TeleportTo(spawn);

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

	if (app->scene->pause) { return true; }

	if (needsToSpawn) 
	{
		TeleportTo(spawn);
		needsToSpawn = false;
	}
	
	if (jumping)
	{
		pbody->body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
		pbody->body->ApplyForce(b2Vec2(lastImpulse.x, -lastImpulse.y), pbody->body->GetWorldCenter(), true);
		jumping = false;
	}

	if (stop)
	{
		pbody->body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
		stop = false;
	}

	if (!airborn)
	{
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
		if (pathToPlayer.Count() > 1 && app->scene->player->position.y > 80)
		{
			int dirX = pathToPlayer.At(1)->x - pathToPlayer.At(0)->x;

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
		}
	}

	if (pendingToDelete) 
	{
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
		lastImpulse.x = physB->impulse.x;
		lastImpulse.y = physB->impulse.y;
		jumping = true;
		airborn = true;
	}

	if (physB->ctype == ColliderType::PLATFORM)
	{
		stop = true;
		airborn = false;
	}
}

void Enemy_Walk::DeathAnimation()
{

}
