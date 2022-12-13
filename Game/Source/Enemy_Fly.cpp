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
	texturePath = parameters.attribute("texturepath").as_string();
	level = parameters.attribute("level").as_int();
	InitAnims();
	return true;

}

bool Enemy_Fly::Start() {

	//initialize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateRectangle(PIXEL_TO_METERS(position.x * 10), PIXEL_TO_METERS(position.y * 10), 39, 29, DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;
	dieFx = app->audio->LoadFx("Assets/Audio/Fx/enemy_die.wav");
	b2MassData* data = new b2MassData; data->center = b2Vec2((float)40 / 2, (float)88 / 2); data->I = 0.0f; data->mass = 0.1f;
	pbody->body->SetMassData(data);
	delete data;

	pbody->body->SetGravityScale(0.0f);

	return true;
}

bool Enemy_Fly::Update()
{
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x - (39 / 2));
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y - (29 / 2));
	currentAnim->Update();
	app->render->DrawTexture(texture, position.x, position.y, &(currentAnim->GetCurrentFrame()));

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

	app->render->DrawLine(app->map->MapToScreen(pathToPlayer.At(0)->x, pathToPlayer.At(0)->y).x,
		app->map->MapToScreen(pathToPlayer.At(0)->x, pathToPlayer.At(0)->y).y,
		app->map->MapToScreen(pathToPlayer.At(1)->x, pathToPlayer.At(1)->y).x,
		app->map->MapToScreen(pathToPlayer.At(1)->x, pathToPlayer.At(1)->y).y, 0, 255, 0, 255);


	if (dirX > 0)
	{
		if (pbody->body->GetLinearVelocity().x < speedCap)
		{
			if (currentAnim != &rightMove) currentAnim = &rightMove;

			pbody->body->ApplyForce(b2Vec2(2.0f, 0.0f), pbody->body->GetWorldCenter(), true);
		}
		//app->font->BlitText(200, 200, 0, "Must go right");
	}
	else if (dirX < 0)
	{
		if (pbody->body->GetLinearVelocity().x > -speedCap)
		{
			if (currentAnim != &leftMove) currentAnim = &leftMove;

			pbody->body->ApplyForce(b2Vec2(-2.0f, 0.0f), pbody->body->GetWorldCenter(), true);
		}
		//app->font->BlitText(200, 200, 0, "Must go left");
	}
	else
	{
		//app->font->BlitText(200, 350, 0, "X is 0;");
		pbody->body->ApplyForce(b2Vec2(-pbody->body->GetLinearVelocity().x * 0.1f, 0.0f), pbody->body->GetWorldCenter(), true);
	}

	if (dirY > 0)
	{
		if (pbody->body->GetLinearVelocity().y < speedCap)
		{
			pbody->body->ApplyForce(b2Vec2(0.0f, 2.0f), pbody->body->GetWorldCenter(), true);
		}
		//app->font->BlitText(300, 200, 0, "Must go down");
	}
	else if (dirY < 0)
	{
		if (pbody->body->GetLinearVelocity().y > -speedCap)
		{
			pbody->body->ApplyForce(b2Vec2(0.0f, -2.0f), pbody->body->GetWorldCenter(), true);
		}
		//app->font->BlitText(300, 200, 0, "Must go up");
	}
	else
	{
		//app->font->BlitText(200, 350, 0, "Y is 0;");
		pbody->body->ApplyForce(b2Vec2(0.0f, -pbody->body->GetLinearVelocity().y * 0.1f), pbody->body->GetWorldCenter(), true);
	}

	//app->font->BlitText(200, 300, 0, std::to_string(dirX).c_str());
	//app->font->BlitText(200, 310, 0, std::to_string(dirY).c_str());
	//app->font->BlitText(200, 320, 0, std::to_string(pbody->body->GetLinearVelocity().x).c_str());
	//app->font->BlitText(200, 330, 0, std::to_string(pbody->body->GetLinearVelocity().y).c_str());

	if (pendingToDelete) { Disable(); }


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