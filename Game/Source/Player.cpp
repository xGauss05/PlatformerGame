#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene_Level1.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Fonts.h"

#include<iostream>
using namespace std;

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

bool Player::Awake() {

	//L02: DONE 1: Initialize Player parameters
	//pos = position;
	//texturePath = "Assets/Textures/player/idle1.png";

	//L02: DONE 5: Get Player parameters from XML
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	dieFx = app->audio->LoadFx("Assets/Audio/Fx/hurt.wav");
	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	isDead = false;
	lifes = 0;
	pbody = app->physics->CreateRectangle(200, 540, 20, 20, DYNAMIC);
	pbody->body->SetFixedRotation(true);
	return true;
}

bool Player::Update()
{
	//Jump
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, 0.0f));
		pbody->body->ApplyForce(b2Vec2(0, -jumpForce), pbody->body->GetWorldCenter(), true);
	}

	//Left
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		if (pbody->body->GetLinearVelocity().x > 0.5f)
		{
			//Opposite direction dampening
			pbody->body->ApplyForce(b2Vec2(-movementDampen, 0.0f), pbody->body->GetWorldCenter(), true);
		}
		else
		{
			if (pbody->body->GetLinearVelocity().x > -speedCap)
				pbody->body->ApplyForce(b2Vec2(-movementForce, 0.0f), pbody->body->GetWorldCenter(), true);
		}
	}

	//Right
	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		if (pbody->body->GetLinearVelocity().x < -0.5f)
		{
			//Opposite direction dampening
			pbody->body->ApplyForce(b2Vec2(movementDampen, 0.0f), pbody->body->GetWorldCenter(), true);
		}
		else
		{
			if (pbody->body->GetLinearVelocity().x < speedCap)
				pbody->body->ApplyForce(b2Vec2(movementForce, 0.0f), pbody->body->GetWorldCenter(), true);
		}
	}

	//General dampening
	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE)
	{
		if (pbody->body->GetLinearVelocity().x > 0.5f)
			pbody->body->ApplyForce(b2Vec2(-pbody->body->GetLinearVelocity().x * idleDampenMultiplier, 0.0f), pbody->body->GetWorldCenter(), true);
		if (pbody->body->GetLinearVelocity().x < -0.5f)
			pbody->body->ApplyForce(b2Vec2(-pbody->body->GetLinearVelocity().x * idleDampenMultiplier, 0.0f), pbody->body->GetWorldCenter(), true);
	}

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x - (width / 2));
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y - (height / 2));
	app->render->DrawTexture(texture, position.x, position.y);

	//(This should be in the debug, pending to move)
	//app->font->BlitText(10, 110, 0, "X SPEED");
	//app->font->BlitText(100, 110, 0, std::to_string(pbody->body->GetLinearVelocity().x).c_str());

	//OnCollision with boxes in the Trigger layer
	for (b2Contact* contact = app->physics->world->GetContactList(); contact; contact = contact->GetNext()) {
		if (contact->GetFixtureB()->IsSensor())
		{
			app->font->BlitText(20, 20, 0, "TOUCHING");
			this->pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, -5.0f));
		}
	}

	return true;
}

bool Player::CleanUp()
{

	return true;
}

void Player::deathAnimation() {

	app->audio->PlayFx(dieFx);

	/*if (healthPoints <= 0) {

		App->particles->AddParticle(App->particles->cardDeath, position.x, position.y);

		App->player->lifePlayer++;

		SetToDelete();
	}*/
}


