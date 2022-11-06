#include "Saw.h"
#include "Textures.h"
#include "Physics.h"
#include "App.h"
#include "Scene_Level1.h"
#include "Point.h"
#include "Input.h"

Saw::Saw() : Entity(EntityType::SAW) 
{
	name.Create("saw");
}

Saw::~Saw() 
{

}

bool Saw::Awake()
{
	return true;
}

bool Saw::Start() 
{
	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateCircle(position.x, position.y, 10, STATIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::SAW;
	return true;
}

bool Saw::Update()
{
	return true;
}

bool Saw::CleanUp()
{
	return true;
}

void Saw::OnCollision(PhysBody* physA, PhysBody* physB) 
{

}

void Saw::DeathAnimation() 
{

}
