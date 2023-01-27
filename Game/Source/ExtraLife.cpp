#include "ExtraLife.h"
#include "Textures.h"
#include "Physics.h"
#include "App.h"
#include "Scene_Level1.h"
#include "Point.h"
#include "Input.h"

ExtraLife::ExtraLife() : Entity(EntityType::EXTRALIFE)
{
	name.Create("extralife");
}

ExtraLife::~ExtraLife()
{

}

bool ExtraLife::Awake()
{
	return true;
}

bool ExtraLife::Start()
{
	//initilize textures
	//texture = app->tex->Load("Assets/Textures/??.png");
	//pbody = app->physics->CreateCircle(position.x, position.y, 10, STATIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::EXTRALIFE;
	return true;
}

bool ExtraLife::Update()
{
	return true;
}

bool ExtraLife::CleanUp()
{
	app->tex->UnLoad(texture);
	return true;
}

void ExtraLife::OnCollision(PhysBody* physA, PhysBody* physB)
{

}

void ExtraLife::DeathAnimation()
{

}
