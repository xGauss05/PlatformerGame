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

bool Enemy_Walk::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	return true;
}

bool Enemy_Walk::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateRectangle(PIXEL_TO_METERS(position.x * 10), PIXEL_TO_METERS(position.y * 10), 40, 88, DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;

	b2MassData* data = new b2MassData; data->center = b2Vec2((float)40 / 2, (float)88 / 2); data->I = 0.0f; data->mass = 0.1f;
	pbody->body->SetMassData(data);
	delete data;

	return true;
}

bool Enemy_Walk::Update()
{
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x - (40 / 2));
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y - (88 / 2));
	app->render->DrawTexture(texture, position.x, position.y);


	if (jumping)
	{
		pbody->body->ApplyForce(b2Vec2(30.0f, -40.0f), pbody->body->GetWorldCenter(), true);
		jumping = false;
	}

	if (pendingToDelete) { Disable(); }

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
			pendingToDelete = true;
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