#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene_Level1.h"
#include "Scene_Win.h"
#include "Scene_Die.h"
#include "Scene_Menu.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Fonts.h"
#include "List.h"
#include "Debug.h"
#include "FadeToBlack.h"
#include "UserInterface.h"

#include<iostream>

using namespace std;

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player()
{

}

void Player::InitAnims()
{
	//Right idle
	for (pugi::xml_node node = parameters.child("right_idle").child("pushback");
		node; node = node.next_sibling("pushback"))
	{
		rightIdle.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}

	//Right run
	for (pugi::xml_node node = parameters.child("right_run").child("pushback");
		node; node = node.next_sibling("pushback"))
	{
		rightRun.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}
	rightRun.speed = parameters.child("right_run").attribute("animspeed").as_float();
	rightRun.loop = parameters.child("right_run").attribute("loop").as_bool();

	//Right jump
	for (pugi::xml_node node = parameters.child("right_jump").child("pushback");
		node; node = node.next_sibling("pushback"))
	{
		rightJump.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}
	rightJump.speed = parameters.child("right_jump").attribute("animspeed").as_float();
	rightJump.loop = parameters.child("right_jump").attribute("loop").as_bool();

	//Right fall
	for (pugi::xml_node node = parameters.child("right_fall").child("pushback");
		node; node = node.next_sibling("pushback"))
	{
		rightFall.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}

	//Right wall
	for (pugi::xml_node node = parameters.child("right_wall").child("pushback");
		node; node = node.next_sibling("pushback"))
	{
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
		node; node = node.next_sibling("pushback"))
	{
		leftRun.PushBack({ node.attribute("x").as_int(),
						   node.attribute("y").as_int(),
						   node.attribute("width").as_int(),
						   node.attribute("height").as_int() });
	}
	leftRun.speed = parameters.child("left_run").attribute("animspeed").as_float();
	leftRun.loop = parameters.child("left_run").attribute("loop").as_bool();

	//Left jump
	for (pugi::xml_node node = parameters.child("left_jump").child("pushback");
		node; node = node.next_sibling("pushback"))
	{
		leftJump.PushBack({ node.attribute("x").as_int(),
						   node.attribute("y").as_int(),
						   node.attribute("width").as_int(),
						   node.attribute("height").as_int() });
	}
	leftJump.speed = parameters.child("left_jump").attribute("animspeed").as_float();
	leftJump.loop = parameters.child("left_jump").attribute("loop").as_bool();

	//Left fall
	for (pugi::xml_node node = parameters.child("left_fall").child("pushback");
		node; node = node.next_sibling("pushback"))
	{
		leftFall.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}

	//Left wall
	for (pugi::xml_node node = parameters.child("left_wall").child("pushback");
		node; node = node.next_sibling("pushback"))
	{
		leftWall.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}

	currentAnim = &rightIdle;
}

bool Player::Awake()
{

	LOG("Loading Player");
	level = 1;
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	dashTexturePath = parameters.child("dash").attribute("texturepath").as_string();
	maxJumps = parameters.attribute("maxJumps").as_int();
	dieFx = app->audio->LoadFx(parameters.attribute("dieFxpath").as_string());
	jumpFx = app->audio->LoadFx(parameters.attribute("jumpFxpath").as_string());
	landingFx = app->audio->LoadFx(parameters.attribute("landingFxpath").as_string());
	goalFx = app->audio->LoadFx(parameters.attribute("goalFxpath").as_string());
	dashFx = app->audio->LoadFx(parameters.attribute("dashFxpath").as_string());
	InitAnims();

	return true;
}

bool Player::Start()
{
	isDead = false;
	dashAvailable = true;
	texture = app->tex->Load(texturePath);
	dashSkill = app->tex->Load(dashTexturePath);
	currentJumps = maxJumps;
	spawn.x = 190;
	spawn.y = 680;
	pbody = app->physics->CreateRectangle(0, 0, 40, height, DYNAMIC);
	pbody->listener = this;
	pbody->body->SetFixedRotation(true);
	pbody->ctype = ColliderType::PLAYER;
	dashIndicator = 3;
	//Changing the body's mass to fit the game physics
	b2MassData* data = new b2MassData; data->center = b2Vec2((float)width / 2, (float)height / 2); data->I = 0.0f; data->mass = 0.390625f;
	pbody->body->SetMassData(data);
	delete data;
	TeleportTo(spawn);

	return true;
}

void Player::AnimationLogic()
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
		if (app->debug->debug && app->debug->variables)
			app->font->BlitText(20, app->debug->varBox + 160, 0, "Right Idle ");

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
		if (app->debug->debug && app->debug->variables)
			app->font->BlitText(20, app->debug->varBox + 160, 0, "Left Idle ");

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
		if (app->debug->debug && app->debug->variables)
			app->font->BlitText(20, app->debug->varBox + 160, 0, "Right Run ");

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
		if (app->debug->debug && app->debug->variables)
			app->font->BlitText(20, app->debug->varBox + 160, 0, "Left Run ");

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
		if (app->debug->debug && app->debug->variables)
			app->font->BlitText(20, app->debug->varBox + 160, 0, "Right Jump ");

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
		if (app->debug->debug && app->debug->variables)
			app->font->BlitText(20, app->debug->varBox + 160, 0, "Left Jump ");

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
		if (app->debug->debug && app->debug->variables)
			app->font->BlitText(20, app->debug->varBox + 160, 0, "Right Fall ");

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
		if (app->debug->debug && app->debug->variables)
			app->font->BlitText(20, app->debug->varBox + 160, 0, "Left Fall ");

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
		if (app->debug->debug && app->debug->variables)
			app->font->BlitText(20, app->debug->varBox + 160, 0, "Right Wall ");

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
		if (app->debug->debug && app->debug->variables)
			app->font->BlitText(20, app->debug->varBox + 160, 0, "Left Wall ");

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

void Player::MovementLogic()
{
	if (!app->debug->godMode || !app->scene->pause)
	{
		//Jump
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && currentJumps > 0)
		{
			app->audio->PlayFx(jumpFx);
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
			//Dash
			if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN && dashAvailable) {
				pbody->body->SetLinearVelocity(b2Vec2(-25.0f, 0));
				pbody->body->SetGravityScale(0);
				app->audio->PlayFx(dashFx);
				start = high_resolution_clock::now();
				dashAvailable = false;
				dashing = true;
			}

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
			//Dash
			if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN && dashAvailable) {
				pbody->body->SetLinearVelocity(b2Vec2(25.0f, 0));
				pbody->body->SetGravityScale(0);
				app->audio->PlayFx(dashFx);
				start = high_resolution_clock::now();
				dashAvailable = false;
				dashing = true;
			}

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
	}
	else
	{
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			pbody->body->SetLinearVelocity(b2Vec2(0.0f, -5.0f));

		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_UP)
			pbody->body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			pbody->body->SetLinearVelocity(b2Vec2(-5.0f, 0.0f));

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_UP)
			pbody->body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));

		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			pbody->body->SetLinearVelocity(b2Vec2(0.0f, 5.0f));

		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_UP)
			pbody->body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			pbody->body->SetLinearVelocity(b2Vec2(5.0f, 0.0f));

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_UP)
			pbody->body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
	}

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x - (width / 2));
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y - (height / 2));
}

void Player::NormalsCheck()
{
	//Raycast from player to collision

	bool playerCollision = false;

	for (b2Contact* contact = app->physics->world->GetContactList(); contact; contact = contact->GetNext())
	{

		ListItem<PhysBody*>* c = app->scene->boxes.start;

		while (c != NULL)
		{
			if (contact->GetFixtureB() == this->pbody->body->GetFixtureList())
			{
				//Check collisions only with the player
				if (contact->GetFixtureA() == c->data->body->GetFixtureList())
				{
					playerCollision = true;

					if (app->debug->hitboxes)
					{
						app->render->DrawLine(METERS_TO_PIXELS(this->pbody->body->GetPosition().x),
							METERS_TO_PIXELS(this->pbody->body->GetPosition().y),
							METERS_TO_PIXELS(c->data->body->GetPosition().x),
							METERS_TO_PIXELS(c->data->body->GetPosition().y),
							0, 255, 0, 255);
					}

					b2RayCastInput input;
					b2RayCastOutput output;

					input.p1.Set(this->pbody->body->GetPosition().x, this->pbody->body->GetPosition().y);
					input.p2.Set(c->data->body->GetPosition().x, c->data->body->GetPosition().y);
					input.maxFraction = 1.0f;

					c->data->body->GetFixtureList()->RayCast(&output, input, 0);

					if (c->data->ctype == ColliderType::LIMIT)
					{
						normal_x = 0.0f;
						normal_y = 0.0f;
					}
					else
					{
						normal_x = output.normal.x;
						normal_y = output.normal.y;
					}
				}
			}
			c = c->next;
		}
	}

	if (playerCollision == true)
	{
		if (normal_x == 0.0f && normal_y == -1.0f) { currentJumps = maxJumps; }
		if (normal_x == 1.0f && normal_y == 0.0f) { currentJumps = maxJumps; }
		if (normal_x == -1.0f && normal_y == 0.0f) { currentJumps = maxJumps; }
	}
	else
	{
		normal_x = 0.0f; normal_y = 0.0f;
	}
}

void Player::SetSpawn(iPoint position, iPoint cameraPosition)
{
	if (spawn.x != position.x || spawn.y != position.y)
	{
		spawn = position;
	}

	if (!app->debug->debugCamera)
	{
		if (app->render->camera.x != cameraPosition.x ||
			app->render->camera.y != cameraPosition.y)
		{
			app->render->camera.x = cameraPosition.x;
			app->render->camera.y = cameraPosition.y;
		}
	}
}

void Player::ReadSpawn()
{
	pugi::xml_document gameSaveFile;
	pugi::xml_parse_result result = gameSaveFile.load_file("save_game.xml");

	if (result == NULL)
	{
		LOG("Could not load xml file savegame.xml. pugi error: %s", result.description());
	}
	else
	{
		level = gameSaveFile.child("save_state").child("level").attribute("value").as_int();

		iPoint newSpawn;
		newSpawn.x = gameSaveFile.child("save_state").child("spawnPos").attribute("x").as_int();
		newSpawn.y = gameSaveFile.child("save_state").child("spawnPos").attribute("y").as_int();

		iPoint cam;
		cam.x = app->render->camera.x;
		cam.y = app->render->camera.y;

		SetSpawn(newSpawn, cam);
	}
}

void Player::ReadKeycard()
{
	pugi::xml_document gameSaveFile;
	pugi::xml_parse_result result = gameSaveFile.load_file("save_game.xml");

	if (result == NULL)
	{
		LOG("Could not load xml file savegame.xml. pugi error: %s", result.description());
	}
	else
	{
		bool keycard;

		keycard = gameSaveFile.child("save_state").child("keycard").attribute("value").as_bool();

		hasKeyCard = keycard;
	}
}

void Player::SaveProgress()
{
	pugi::xml_document* saveDoc = new pugi::xml_document();
	pugi::xml_node saveStateNode = saveDoc->append_child("save_state");

	pugi::xml_node levelNode = saveStateNode.append_child("level");
	levelNode.append_attribute("value") = this->level;

	pugi::xml_node spawnNode = saveStateNode.append_child("spawnPos");
	spawnNode.append_attribute("x") = spawn.x;
	spawnNode.append_attribute("y") = spawn.y;

	pugi::xml_node keycardNode = saveStateNode.append_child("keycard");
	keycardNode.append_attribute("value") = app->scene->player->hasKeyCard;

	saveDoc->save_file("save_game.xml");
}

void Player::TeleportTo(iPoint position) {
	pbody->body->SetLinearVelocity(b2Vec2(0, 0));
	pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y)), 0.0f);
	pbody->body->ApplyForce(b2Vec2(0.1f, 0.0f), pbody->body->GetWorldCenter(), true);
}

void Player::LevelSelector()
{
	iPoint newSpawnPoint;
	iPoint newCameraPosition;
	switch (level)
	{

	case 1:
		
		newCameraPosition.x = 0;
		newCameraPosition.y = 0;
		if (!checkpointReached)
		{
			newSpawnPoint.x = 190;
			newSpawnPoint.y = 680;
			SetSpawn(newSpawnPoint, newCameraPosition);
		}
		else { SetSpawn(spawn, newCameraPosition); }

		break;
	case 2:
		
		newCameraPosition.x = -(1600 + 32);
		newCameraPosition.y = 0;
		if (!checkpointReached)
		{
			newSpawnPoint.x = 1790;
			newSpawnPoint.y = 680;
			SetSpawn(newSpawnPoint, newCameraPosition);
		}
		else { SetSpawn(spawn, newCameraPosition); }
		

		break;
	case 3:
		
		newCameraPosition.x = -(3200 + 64);
		newCameraPosition.y = 0;
		if (!checkpointReached)
		{
			newSpawnPoint.x = 3300;
			newSpawnPoint.y = 680;
			SetSpawn(newSpawnPoint, newCameraPosition);
		}
		else { SetSpawn(spawn, newCameraPosition); }

		break;
	case 4:
		
		newCameraPosition.x = -(4800 + 96);
		newCameraPosition.y = 0;
		if (!checkpointReached)
		{
			newSpawnPoint.x = 4930;
			newSpawnPoint.y = 70;
			SetSpawn(newSpawnPoint, newCameraPosition);
		}
		else { SetSpawn(spawn, newCameraPosition); }

		break;
	case 5:
		app->ftb->SceneFadeToBlack(app->scene, app->scene_win, 50.0f);
		break;
	default:
		break;
	}
}

void Player::ResetGame() {
	spawn.x = 190;
	spawn.y = 680;
	level = 1;
	LevelSelector();
	TeleportTo(spawn);
}

bool Player::Update()
{
	AnimationLogic(); //This call must go before decreasing currentJumps in order for jump animations to trigger properly
	MovementLogic();
	NormalsCheck();
	LevelSelector();

	currentAnim->Update();
	app->render->DrawTexture(texture, position.x, position.y, &(currentAnim->GetCurrentFrame()));

	currentTime = high_resolution_clock::now();
	dashDuration = dashCooldown = duration_cast<milliseconds>(currentTime - start);

	if (dashDuration >= milliseconds(250) && dashing) {
		pbody->body->SetLinearVelocity(b2Vec2(0, 0));
		pbody->body->SetGravityScale(1);
		dashing = false;
	}

	int filling = 100;
	if (dashCooldown.count() < 3000 && !dashAvailable) {
		//SDL_SetTextureAlphaMod(dashSkill, (float)((dashCooldown.count() / 3000.0f)) * 255.0f);
		filling = (dashCooldown.count() / 3000.0f) * 100;
	}
	else {
		dashAvailable = true;
	}

	SDL_Rect rect({ 1450,780,filling,100 });
	app->render->DrawRectangle(rect, 10, 10, 10, 150, true, false);
	app->render->DrawTexture(dashSkill, -app->render->camera.x + 1450, -app->render->camera.y + 780, NULL);

	if (doorReached)
	{
		dashAvailable = true;
		app->entityManager->ActivateEnemies();
		TeleportTo(spawn);
		doorReached = false;
		SaveProgress();
	}

	if (isDead) {
		dashAvailable = true;
		ReadKeycard();
		app->entityManager->ReviveAllEntities();
		app->entityManager->NeedsToSpawnAllEntities();
		app->entityManager->ActivateEnemies();
		app->audio->PlayFx(dieFx);
		app->entityManager->Disable();

		if (lives > 0)
		{
			lives--;
			TeleportTo(spawn);
			//app->ftb->SceneFadeToBlack(app->scene, app->scene_die, 0);
		}
		else
		{
			iPoint zero; zero.x = 0; zero.y = 0;
			TeleportTo(zero);

			lives = 5;
		
			app->entityManager->ResetCheckpoints();
			checkpointReached = false;


			LOG("Main menu button click.");
			app->entityManager->ReviveAllEntities();
			app->entityManager->NeedsToSpawnAllEntities();
			ResetGame();
			app->scene->pause = false;
			app->ftb->SceneFadeToBlack(app->scene, app->scene_menu, 0.0f);
			app->scene->pauseBtn->state = GuiControlState::DISABLED;
			app->scene->resumeBtn->state = GuiControlState::DISABLED;
			app->scene->settingsBtn->state = GuiControlState::DISABLED;
			app->scene->backToTitleBtn->state = GuiControlState::DISABLED;
			app->scene->exitBtn->state = GuiControlState::DISABLED;
		}

		isDead = false;
	}

	return true;
}

bool Player::CleanUp()
{
	app->tex->UnLoad(texture);

	texturePath = nullptr;

	currentAnim = nullptr;

	RELEASE(pbody);

	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		app->audio->PlayFx(landingFx);
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	case ColliderType::SAW:
		LOG("Collision SAW");
		if (!app->debug->godMode) isDead = true;

		break;
	case ColliderType::GOAL:
		LOG("Collision GOAL");
		if (hasKeyCard) 
		{
			level++;
			app->audio->PlayFx(goalFx);
			if (app->scene->player->level < 5) doorReached = true;
			checkpointReached = false;
			hasKeyCard = false;
			app->ui->StartTimer(30000);
		}
	case ColliderType::EXTRALIFE:
		if (lives < 4) lives++;
		break;
	case ColliderType::KEYCARD:
		hasKeyCard = true;
		break;
	}
}

void Player::DeathAnimation()
{

}
