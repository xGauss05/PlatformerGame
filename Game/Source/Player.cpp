#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

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

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	pbody = app->physics->CreateRectangle(200, 650, 20, 20, DYNAMIC);
	return true;
}

bool Player::Update()
{
	//L02: DONE 4: modify the position of the player using arrow keys and render the texture
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		cout << "Hola\n";
		pbody->body->ApplyForce(b2Vec2(0, -200), pbody->body->GetWorldCenter(), true);
	}

	/*if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		position.y += speed;*/

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		pbody->body->ApplyForce(b2Vec2(-10.0f, 0.0f), pbody->body->GetWorldCenter(), true);
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		pbody->body->ApplyForce(b2Vec2(10.0f, 0.0f), pbody->body->GetWorldCenter(), true);
	}

	//Controlling jump stuff
	if (pbody->body->GetLinearVelocity().y < 0.5f && pbody->body->GetLinearVelocity().y > -0.5f)
	{
		cout << "Ground\n";
		pbody->body->SetLinearDamping(2);
	}
	else
	{
		cout << "Air\n";
		pbody->body->SetLinearDamping(0);
	}

	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x - (width / 2));
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y - (height / 2));
	app->render->DrawTexture(texture, position.x, position.y);

	return true;
}

bool Player::CleanUp()
{

	return true;
}