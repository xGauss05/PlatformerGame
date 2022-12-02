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
	pbody = app->physics->CreateRectangle(PIXEL_TO_METERS(5000), PIXEL_TO_METERS(5000), 40, 88, DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;

	b2MassData* data = new b2MassData; data->center = b2Vec2((float)40 / 2, (float)88 / 2); data->I = 0.0f; data->mass = 0.1f;
	pbody->body->SetMassData(data);
	delete data;

	pbody->body->SetGravityScale(0.0f);

	return true;
}

bool Enemy_Fly::Update()
{
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x - (40 / 2));
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y - (88 / 2));
	app->render->DrawTexture(texture, position.x, position.y);

	//Pathfinding

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

	for (uint i = 0; i < pathToPlayer.Count(); ++i)
	{
		iPoint pos = app->map->MapToScreen(pathToPlayer.At(i)->x, pathToPlayer.At(i)->y);
		app->render->DrawTexture(app->debug->playerPathTex, pos.x, pos.y);
	}

	//Movement
	int dirX = pathToPlayer.At(1)->x - pathToPlayer.At(0)->x;
	int dirY = pathToPlayer.At(1)->y - pathToPlayer.At(0)->y;

	if (dirX > 0)
	{
		pbody->body->ApplyForce(b2Vec2(1.0f, 0.0f), pbody->body->GetWorldCenter(), true);
	}
	else
	{
		pbody->body->ApplyForce(b2Vec2(-1.0f, 0.0f), pbody->body->GetWorldCenter(), true);
	}
	if (dirY > 0)
	{
		pbody->body->ApplyForce(b2Vec2(0.0f, 1.0f), pbody->body->GetWorldCenter(), true);
	}
	else
	{
		pbody->body->ApplyForce(b2Vec2(0.0f, -1.0f), pbody->body->GetWorldCenter(), true);
	}

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