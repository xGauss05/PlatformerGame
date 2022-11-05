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

void Player::initAnims()
{
	//Right idle
	for (pugi::xml_node node = parameters.child("right_idle").child("pushback");
		node; node = node.next_sibling("pushback")) {
		rightIdle.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}

	//Right run
	for (pugi::xml_node node = parameters.child("right_run").child("pushback");
		node; node = node.next_sibling("pushback")) {
		rightRun.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}
	rightRun.speed = parameters.child("right_run").attribute("animspeed").as_float();
	rightRun.loop = parameters.child("right_run").attribute("loop").as_bool();

	//Right jump
	for (pugi::xml_node node = parameters.child("right_jump").child("pushback");
		node; node = node.next_sibling("pushback")) {
		rightJump.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}
	rightJump.speed = parameters.child("right_jump").attribute("animspeed").as_float();
	rightJump.loop = parameters.child("right_jump").attribute("loop").as_bool();

	//Right fall
	for (pugi::xml_node node = parameters.child("right_fall").child("pushback");
		node; node = node.next_sibling("pushback")) {
		rightFall.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}
	//Right wall
	for (pugi::xml_node node = parameters.child("right_wall").child("pushback");
		node; node = node.next_sibling("pushback")) {
		rightWall.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}

	//Left idle
	for (pugi::xml_node node = parameters.child("left_idle").child("pushback");
		node; node = node.next_sibling("pushback")) {
		leftIdle.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}

	//Left run
	for (pugi::xml_node node = parameters.child("left_run").child("pushback");
		node; node = node.next_sibling("pushback")) {
		leftRun.PushBack({ node.attribute("x").as_int(),
						   node.attribute("y").as_int(),
						   node.attribute("width").as_int(),
						   node.attribute("height").as_int() });
	}
	leftRun.speed = parameters.child("left_run").attribute("animspeed").as_float();
	leftRun.loop = parameters.child("left_run").attribute("loop").as_bool();

	//Left jump
	for (pugi::xml_node node = parameters.child("left_jump").child("pushback");
		node; node = node.next_sibling("pushback")) {
		leftJump.PushBack({ node.attribute("x").as_int(),
						   node.attribute("y").as_int(),
						   node.attribute("width").as_int(),
						   node.attribute("height").as_int() });
	}
	leftJump.speed = parameters.child("left_jump").attribute("animspeed").as_float();
	leftJump.loop = parameters.child("left_jump").attribute("loop").as_bool();

	//Left fall
	for (pugi::xml_node node = parameters.child("left_fall").child("pushback");
		node; node = node.next_sibling("pushback")) {
		leftFall.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}
	//Left wall
	for (pugi::xml_node node = parameters.child("left_wall").child("pushback");
		node; node = node.next_sibling("pushback")) {
		leftWall.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}

	currentAnim = &rightIdle;
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
	initAnims();
	
	return true;
}

bool Player::Start() {

	texture = app->tex->Load(texturePath);
	isDead = false;
	lifes = 0;
	currentJumps = maxJumps;

	pbody = app->physics->CreateRectangle(100, 450, 40, height, DYNAMIC);

	pbody->listener = (Module*)app->entityManager;
	pbody->body->SetFixedRotation(true);
	pbody->entity = this;

	//Changing the body's mass to fit the game physics
	b2MassData* data = new b2MassData; data->center = b2Vec2((float)width / 2, (float)height / 2); data->I = 0.0f; data->mass = 0.390625f;
	pbody->body->SetMassData(data);
	delete data;

	return true;
}

void Player::animationLogic()
{
	//Two side input "security check"
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			if (normal_y == -1.0f)
			{
				if (pbody->body->GetLinearVelocity().x > 0.01f)
				{
					currentAnim = &rightIdle;
				}
				else if (pbody->body->GetLinearVelocity().x < -0.01f)
				{
					currentAnim = &leftIdle;
				}
			}
			else
			{
				if (pbody->body->GetLinearVelocity().x > 0.01f)
				{
					currentAnim = &rightFall;
				}
				else if (pbody->body->GetLinearVelocity().x < -0.01f)
				{
					currentAnim = &leftFall;
				}
			}
		}
	}

	//Reset jump animation
	if (currentAnim != &rightJump && currentAnim != &leftJump && app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		rightJump.Reset();
		leftJump.Reset();
	}

	if (currentAnim == &rightIdle)
	{
		app->font->BlitText(20, 100, 0, "RIGHT IDLE ");

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			currentAnim = &leftRun;
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			currentAnim = &rightRun;
		}
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			currentAnim = &rightJump;
		}
	}
	if (currentAnim == &leftIdle)
	{
		app->font->BlitText(20, 100, 0, "LEFT IDLE ");

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			currentAnim = &leftRun;
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			currentAnim = &rightRun;
		}
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			currentAnim = &leftJump;
		}
	}
	if (currentAnim == &rightRun)
	{
		app->font->BlitText(20, 100, 0, "RIGHT RUN ");

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_UP)
		{
			if (app->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT)
			{
				currentAnim = &rightIdle;
			}
			else currentAnim = &leftRun;
		}
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
		{
			currentAnim = &leftRun;
		}
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			currentAnim = &rightJump;
		}
		if (normal_y == 0.0f)
		{
			currentAnim = &rightFall;
		}
	}
	if (currentAnim == &leftRun)
	{
		app->font->BlitText(20, 100, 0, "LEFT RUN ");

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_UP)
		{
			if (app->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
			{
				currentAnim = &leftIdle;
			}
			else currentAnim = &rightRun;
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
		{
			currentAnim = &rightRun;
		}
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			currentAnim = &leftJump;
		}
		if (normal_y == 0.0f)
		{
			currentAnim = &leftFall;
		}
	}
	if (currentAnim == &rightJump)
	{
		app->font->BlitText(20, 100, 0, "RIGHT JUMP ");

		if (rightJump.HasFinished())
		{
			if (normal_x == -1.0f)
			{
				currentAnim = &rightWall;
			}
			else if (normal_x == 0.0f)
			{
				currentAnim = &rightFall;
			}
		}
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
		{
			currentAnim = &leftFall;
		}
	}
	if (currentAnim == &leftJump)
	{
		app->font->BlitText(20, 100, 0, "LEFT JUMP ");

		if (leftJump.HasFinished())
		{
			if (normal_x == 1.0f)
			{
				currentAnim = &leftWall;
			}
			else if (normal_x == 0.0f)
			{
				currentAnim = &leftFall;
			}
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
		{
			currentAnim = &rightFall;
		}
	}
	if (currentAnim == &rightFall)
	{
		app->font->BlitText(20, 100, 0, "RIGHT FALL ");

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
		{
			currentAnim = &leftFall;
		}
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && currentJumps > 0)
		{
			currentAnim = &rightJump;
		}
		if (normal_y == -1.0f)
		{
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			{
				currentAnim = &rightRun;
			}
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE)
			{
				currentAnim = &rightIdle;
			}
		}

		if (normal_x == -1.0f)
		{
			currentAnim = &rightWall;
		}
		else if (normal_x == 1.0f)
		{
			currentAnim = &leftWall;
		}
	}
	if (currentAnim == &leftFall)
	{
		app->font->BlitText(20, 100, 0, "LEFT FALL ");

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
		{
			currentAnim = &rightFall;
		}
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && currentJumps > 0)
		{
			currentAnim = &leftJump;
		}
		if (normal_y == -1.0f)
		{
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{
				currentAnim = &leftRun;
			}
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE)
			{
				currentAnim = &leftIdle;
			}
		}

		if (normal_x == 1.0f)
		{
			currentAnim = &leftWall;
		}
		else if (normal_x == -1.0f)
		{
			currentAnim = &rightWall;
		}
	}
	if (currentAnim == &rightWall)
	{
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{
				currentAnim = &leftJump;
			}
			else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			{
				currentAnim = &rightJump;
			}
		}
		if (normal_x == 0.0f)
		{
			currentAnim = &leftFall;
		}
	}
	if (currentAnim == &leftWall)
	{
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{
				currentAnim = &leftJump;
			}
			else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			{
				currentAnim = &rightJump;
			}
		}
		if (normal_x == 0.0f)
		{
			currentAnim = &rightFall;
		}
	}
}
void Player::movementLogic()
{
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
}
void Player::normalsCheck()
{
	//Raycast from player to collision
	if (app->physics->world->GetContactList() == NULL) { normal_x = 0.0f; normal_y = 0.0f; }

	for (b2Contact* contact = app->physics->world->GetContactList(); contact; contact = contact->GetNext()) {

		ListItem<PhysBody*>* c = app->scene->boxes.start;

		while (c != NULL)
		{
			if (contact->GetFixtureA() == c->data->body->GetFixtureList())
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
}
void Player::levelSelector()
{
	//Pass level
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
}

bool Player::Update()
{
	animationLogic(); //This call must go before decreasing currentJumps in order for jump animations to trigger properly
	movementLogic();
	normalsCheck();
	levelSelector();

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


