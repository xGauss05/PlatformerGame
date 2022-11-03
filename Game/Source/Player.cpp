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
#include "List.h"

#include<iostream>

using namespace std;

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

bool Player::Awake() {

	LOG("Loading Player");
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	maxJumps = parameters.attribute("maxJumps").as_int();
	dieFx = app->audio->LoadFx(parameters.attribute("dieFxpath").as_string());

	for (pugi::xml_node node = parameters.child("right_idle").child("pushback");
		node; node = node.next_sibling("pushback")) {
		rightIdle.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}
	
	for (pugi::xml_node node = parameters.child("right_run").child("pushback");
		node; node = node.next_sibling("pushback")) {
		rightRun.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}
	rightRun.speed = parameters.child("right_run").attribute("animspeed").as_float();
	rightRun.loop = parameters.child("right_run").attribute("loop").as_bool();

	for (pugi::xml_node node = parameters.child("left_idle").child("pushback");
		node; node = node.next_sibling("pushback")) {
		leftIdle.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}
	for (pugi::xml_node node = parameters.child("left_run").child("pushback");
		node; node = node.next_sibling("pushback")) {
		leftRun.PushBack({ node.attribute("x").as_int(),
						   node.attribute("y").as_int(),
						   node.attribute("width").as_int(),
						   node.attribute("height").as_int() });
	}
	leftRun.speed = parameters.child("left_run").attribute("animspeed").as_float();
	leftRun.loop = parameters.child("left_run").attribute("loop").as_bool();

	currentAnim = &rightIdle;
	return true;
}

bool Player::Start() {

	texture = app->tex->Load(texturePath);
	isDead = false;
	lifes = 0;
	currentJumps = maxJumps;

	pbody = app->physics->CreateRectangle(100, 450, 40, height, DYNAMIC);
	//headSensor = app->physics->CreateRectangleSensor(200, 540, 40, 10, STATIC);
	//wallSensorL = app->physics->CreateRectangleSensor(200, 540, 10, 82, STATIC);
	//wallSensorR = app->physics->CreateRectangleSensor(200, 540, 10, 82, STATIC);
	//footSensor = app->physics->CreateRectangleSensor(200, 540, 40, 10, STATIC);

	//headSensor->listener = (Module*)app->entityManager;
	//wallSensorL->listener = (Module*)app->entityManager;
	//wallSensorR->listener = (Module*)app->entityManager;
	//footSensor->listener = (Module*)app->entityManager;

	pbody->listener = (Module*)app->entityManager;
	pbody->body->SetFixedRotation(true);
	pbody->entity = this;

	//Changing the body's mass to fit the game physics
	b2MassData* data = new b2MassData; data->center = b2Vec2((float)width / 2, (float)height / 2); data->I = 0.0f; data->mass = 0.390625f;
	pbody->body->SetMassData(data);
	delete data;

	return true;
}

bool Player::Update()
{
	//headSensor->body->SetTransform(b2Vec2(this->pbody->body->GetPosition().x, this->pbody->body->GetPosition().y - 1.6f), 0.0f);
	//footSensor->body->SetTransform(b2Vec2(this->pbody->body->GetPosition().x, this->pbody->body->GetPosition().y + 1.6f), 0.0f);
	//wallSensorL->body->SetTransform(b2Vec2(this->pbody->body->GetPosition().x - 0.83f, this->pbody->body->GetPosition().y), 0.0f);
	//wallSensorR->body->SetTransform(b2Vec2(this->pbody->body->GetPosition().x + 0.85f, this->pbody->body->GetPosition().y), 0.0f);

	//Jump
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && currentJumps > 0)
	{
		pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, 0.0f));
		pbody->body->ApplyForce(b2Vec2(0, -jumpForce), pbody->body->GetWorldCenter(), true);
		currentJumps--;

		//Right of the wall
		if (normal_x == 1.0f && normal_y == 0.0f) { pbody->body->ApplyForce(b2Vec2(jumpForce / 2, 0.0f), pbody->body->GetWorldCenter(), true); }
		//Left of the wall
		if (normal_x == -1.0f && normal_y == 0.0f) { pbody->body->ApplyForce(b2Vec2(-jumpForce / 2, 0.0f), pbody->body->GetWorldCenter(), true); }
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

	if (app->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
	{
		if (level < 2)
		{
			level++;
			app->render->camera.x -= (1600 + 32);
			pbody->body->ApplyForce(b2Vec2(0.1f, 0.0f), pbody->body->GetWorldCenter(), true);

			switch (level)
			{
			case 2:
				pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(2400), PIXEL_TO_METERS(70)), 0.0f);
				break;
			default:
				break;
			}
		}
	}


	//(This should be in the debug, pending to move)
	//app->font->BlitText(10, 110, 0, "X SPEED");
	//app->font->BlitText(100, 110, 0, std::to_string(pbody->body->GetLinearVelocity().x).c_str());

	//Raycast from player to collision
	if (app->physics->world->GetContactList() == NULL) { normal_x = 0.0f; normal_y = 0.0f; }

	for (b2Contact* contact = app->physics->world->GetContactList(); contact; contact = contact->GetNext()) {

		ListItem<PhysBody*>* c = app->scene->boxes.start;

		while (c != NULL)
		{
			if (contact->GetFixtureB() == c->data->body->GetFixtureList())
			{
				app->render->DrawLine(METERS_TO_PIXELS(this->pbody->body->GetPosition().x),
					METERS_TO_PIXELS(this->pbody->body->GetPosition().y),
					METERS_TO_PIXELS(c->data->body->GetPosition().x),
					METERS_TO_PIXELS(c->data->body->GetPosition().y),
					0, 255, 0, 255);

				b2RayCastInput input;
				b2RayCastOutput output;

				input.p1.Set(this->pbody->body->GetPosition().x, this->pbody->body->GetPosition().y);
				input.p2.Set(c->data->body->GetPosition().x, c->data->body->GetPosition().y);
				input.maxFraction = 1.0f;

				c->data->body->GetFixtureList()->RayCast(&output, input, 0);
				normal_x = output.normal.x;
				normal_y = output.normal.y;
			}
			c = c->next;
		}
	}

	if (normal_x == 0.0f && normal_y == 0.0f) { app->font->BlitText(20, 20, 0, "AIR"); }
	if (normal_x == 0.0f && normal_y == 1.0f) { app->font->BlitText(20, 20, 0, "CEILING"); }
	if (normal_x == 0.0f && normal_y == -1.0f) { app->font->BlitText(20, 20, 0, "GROUND"); currentJumps = maxJumps; }
	if (normal_x == 1.0f && normal_y == 0.0f) { app->font->BlitText(20, 20, 0, "RIGHT"); currentJumps = maxJumps; }
	if (normal_x == -1.0f && normal_y == 0.0f) { app->font->BlitText(20, 20, 0, "LEFT"); currentJumps = maxJumps; }

	app->font->BlitText(20, 60, 0, "X NORMAL. ");
	app->font->BlitText(100, 60, 0, std::to_string(normal_x).c_str());
	app->font->BlitText(20, 70, 0, "Y NORMAL. ");
	app->font->BlitText(100, 70, 0, std::to_string(normal_y).c_str());

	app->font->BlitText(20, 80, 0, "JUMPS. ");
	app->font->BlitText(100, 80, 0, std::to_string(currentJumps).c_str());

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

}

void Player::deathAnimation() {

	app->audio->PlayFx(dieFx);

	/*if (healthPoints <= 0) {

		App->particles->AddParticle(App->particles->cardDeath, position.x, position.y);

		App->player->lifePlayer++;

		SetToDelete();
	}*/
}


