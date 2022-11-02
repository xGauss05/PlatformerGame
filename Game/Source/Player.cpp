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

void Player::initAnims()
{
	//Idle "animations"
	rightIdle.PushBack({ 0,0,64,88 });
	leftIdle.PushBack({ 0,88,64,88 });

	//Running right
	for (int i = 0; i < 13; i++)
	{
		rightRun.PushBack({ i * 64,0,64,88 });
	}
	rightRun.speed = 0.4f;
	rightRun.loop = true;

	//Running left
	for (int i = 0; i < 13; i++)
	{
		leftRun.PushBack({ i * 64,88,64,88 });
	}
	leftRun.speed = 0.4f;
	leftRun.loop = true;

	currentAnim = &rightIdle;
}

Player::~Player() {

}

bool Player::Awake() {

	//L02: DONE 5: Get Player parameters from XML
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	dieFx = app->audio->LoadFx("Assets/Audio/Fx/hurt.wav");
	return true;
}

bool Player::Start() {

	texture = app->tex->Load(texturePath);
	isDead = false;
	lifes = 0;
	jumps = 1;

	pbody = app->physics->CreateRectangle(100, 450, 40, height, DYNAMIC);
	headSensor = app->physics->CreateRectangleSensor(200, 540, 40, 10, STATIC);
	wallSensorL = app->physics->CreateRectangleSensor(200, 540, 10, 82, STATIC);
	wallSensorR = app->physics->CreateRectangleSensor(200, 540, 10, 82, STATIC);
	footSensor = app->physics->CreateRectangleSensor(200, 540, 40, 10, STATIC);

	headSensor->listener = (Module*)app->entityManager;
	wallSensorL->listener = (Module*)app->entityManager;
	wallSensorR->listener = (Module*)app->entityManager;
	footSensor->listener = (Module*)app->entityManager;

	pbody->listener = (Module*)app->entityManager;
	pbody->body->SetFixedRotation(true);
	pbody->entity = this;

	//Changing the body's mass to fit the game physics
	b2MassData* data = new b2MassData; data->center = b2Vec2((float)width / 2, (float)height / 2); data->I = 0.0f; data->mass = 0.390625f;
	pbody->body->SetMassData(data);
	delete data;

	initAnims();
	return true;
}

bool Player::Update()
{
	headSensor->body->SetTransform(b2Vec2(this->pbody->body->GetPosition().x, this->pbody->body->GetPosition().y - 1.6f), 0.0f);
	footSensor->body->SetTransform(b2Vec2(this->pbody->body->GetPosition().x, this->pbody->body->GetPosition().y + 1.6f), 0.0f);
	wallSensorL->body->SetTransform(b2Vec2(this->pbody->body->GetPosition().x - 0.83f, this->pbody->body->GetPosition().y), 0.0f);
	wallSensorR->body->SetTransform(b2Vec2(this->pbody->body->GetPosition().x + 0.85f, this->pbody->body->GetPosition().y), 0.0f);
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

		currentAnim = &leftRun;
	}
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_UP) { currentAnim = &leftIdle; leftRun.Reset(); }

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

		currentAnim = &rightRun;
	}
	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_UP) { currentAnim = &rightIdle; rightRun.Reset(); }

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

	//(This should be in the debug, pending to move)
	//app->font->BlitText(10, 110, 0, "X SPEED");
	//app->font->BlitText(100, 110, 0, std::to_string(pbody->body->GetLinearVelocity().x).c_str());

	//OnCollision with boxes in the Trigger layer
	for (b2Contact* contact = app->physics->world->GetContactList(); contact; contact = contact->GetNext()) {
		if (contact->GetFixtureB()->IsSensor())
		{
			app->font->BlitText(20, 20, 0, "TOUCHING");
			//this->pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, -5.0f));
		}
	}
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

	//Animation Stuff
	currentAnim->Update();
	app->render->DrawTexture(texture, position.x, position.y, &(currentAnim->GetCurrentFrame()));

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


