#include "Checkpoint.h"
#include "Textures.h"
#include "Physics.h"
#include "App.h"
#include "Scene_Level1.h"
#include "Point.h"
#include "Input.h"

Checkpoint::Checkpoint() : Entity(EntityType::CHECKPOINT)
{
	name.Create("checkpoint");
}

Checkpoint::~Checkpoint()
{

}

bool Checkpoint::Awake()
{
	spawn.x = position.x;
	spawn.y = position.y;

	return true;
}

bool Checkpoint::Start()
{
	checkpointTexture = app->tex->Load("Assets/Textures/checkpoint.png");
	pbody = app->physics->CreateRectangleSensor(position.x + 16, position.y + 32, 32, 64, STATIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::CHECKPOINT;
	return true;
}

bool Checkpoint::Update()
{
	SDL_Rect rect;
	rect = { 0,0,32,64 };
	app->render->DrawTexture(checkpointTexture,
		METERS_TO_PIXELS(pbody->body->GetPosition().x) - 16,
		METERS_TO_PIXELS(pbody->body->GetPosition().y) - 32,
		&rect);

	return true;
}

bool Checkpoint::CleanUp()
{
	app->tex->UnLoad(checkpointTexture);
	return true;
}

void Checkpoint::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		app->scene->player->pbody->body->ApplyForce(b2Vec2(0.0f, -5000.0f), pbody->body->GetWorldCenter(), true);

		break;
	}
}

void Checkpoint::DeathAnimation()
{

}
