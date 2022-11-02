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
	jumps = 1;

	pbody = app->physics->CreateRectangle(200, 540, 20, 20, DYNAMIC);
	headSensor = app->physics->CreateRectangleSensor(200, 540, 20, 10, STATIC);
	wallSensorL = app->physics->CreateRectangleSensor(200, 540, 10, 20, STATIC);
	wallSensorR = app->physics->CreateRectangleSensor(200, 540, 10, 20, STATIC);
	footSensor = app->physics->CreateRectangleSensor(200, 540, 20, 10, STATIC);

	headSensor->listener = (Module*)app->entityManager;
	wallSensorL->listener = (Module*)app->entityManager;
	wallSensorR->listener = (Module*)app->entityManager;
	footSensor->listener = (Module*)app->entityManager;
	
	pbody->listener = (Module*)app->entityManager;
	pbody->body->SetFixedRotation(true);
	pbody->entity = this;
	return true;
}

bool Player::Update()
{
	headSensor->body->SetTransform(b2Vec2(this->pbody->body->GetPosition().x, this->pbody->body->GetPosition().y - 0.5f), 0.0f);
	footSensor->body->SetTransform(b2Vec2(this->pbody->body->GetPosition().x, this->pbody->body->GetPosition().y + 0.5f), 0.0f);
	wallSensorL->body->SetTransform(b2Vec2(this->pbody->body->GetPosition().x - 0.5f, this->pbody->body->GetPosition().y), 0.0f);
	wallSensorR->body->SetTransform(b2Vec2(this->pbody->body->GetPosition().x + 0.5f, this->pbody->body->GetPosition().y), 0.0f);
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
	//for (b2Contact* contact = app->physics->world->GetContactList(); contact; contact = contact->GetNext()) {
	//	if (contact->GetFixtureB()->IsSensor())
	//	{
	//		app->font->BlitText(20, 20, 0, "TOUCHING");
	//		//this->pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, -5.0f));
	//	}

	//	//Mierda Raycast
	//	/*app->render->DrawLine(this->position.x + this->width/2, this->position.y + this->height/2, 
	//						  METERS_TO_PIXELS(contact->GetFixtureB()->GetBody()->GetPosition().x), 
	//						  METERS_TO_PIXELS(contact->GetFixtureB()->GetBody()->GetPosition().y), 
	//						  0, 255, 0, 255);

	//	float direction_X = 0;         
	//	float direction_Y = 0;
	//	this->pbody->RayCast(PIXEL_TO_METERS(this->position.x), PIXEL_TO_METERS(this->position.y),
	//						 contact->GetFixtureB()->GetBody()->GetPosition().x, contact->GetFixtureB()->GetBody()->GetPosition().y,
	//						 direction_X, direction_Y);

	//	app->render->DrawLine(this->position.x + this->width / 2, this->position.y + this->height / 2, 
	//						  direction_X*100, direction_Y*100,
	//						  255, 255, 0, 255);

	//	LOG("X : %s", std::to_string(direction_X).c_str());
	//	LOG("Y : %s", std::to_string(direction_Y).c_str());*/
	//}

	return true;
}

bool Player::CleanUp()
{

	return true;
}

void Player::OnCollision(PhysBody* body) {
	if (body->entity != NULL) {
		if (body->entity->type == EntityType::ITEM) {
			LOG("IM COLLIDING ITEM");
		}
	}
	else {
		if (body == headSensor) {
			LOG("IM COLLIDING MY HEAD");
		}
		if (body == wallSensorL) {
			LOG("IM COLLIDING MY LEFT");
		}
		if (body == wallSensorR) {
			LOG("IM COLLIDING MY RIGHT");
		}
		if (body == footSensor) {
			LOG("IM COLLIDING MY FOOT");
		}
	}
	
}

void Player::deathAnimation() {

	app->audio->PlayFx(dieFx);

	/*if (healthPoints <= 0) {

		App->particles->AddParticle(App->particles->cardDeath, position.x, position.y);

		App->player->lifePlayer++;

		SetToDelete();
	}*/
}


