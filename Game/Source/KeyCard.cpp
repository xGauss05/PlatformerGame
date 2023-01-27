#include "KeyCard.h"
#include "Textures.h"
#include "Physics.h"
#include "App.h"
#include "Scene_Level1.h"
#include "Point.h"
#include "Input.h"

KeyCard::KeyCard() : Entity(EntityType::KEYCARD)
{
	name.Create("saw");
}

KeyCard::~KeyCard()
{

}

bool KeyCard::Awake()
{
	return true;
}

bool KeyCard::Start()
{
	//initilize textures
	//texture = app->tex->Load("Assets/Textures/??.png");
	//pbody = app->physics->CreateCircle(position.x, position.y, 10, STATIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::KEYCARD;
	return true;
}

bool KeyCard::Update()
{
	return true;
}

bool KeyCard::CleanUp()
{
	app->tex->UnLoad(texture);

	texturePath = nullptr;

	return true;
}

void KeyCard::OnCollision(PhysBody* physA, PhysBody* physB)
{

}

void KeyCard::DeathAnimation()
{

}
