#include "Item.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene_Level1.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

Item::Item() : Entity(EntityType::ITEM)
{
	name.Create("item");
}

Item::~Item() 
{

}

bool Item::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	pickUpFx = app->audio->LoadFx("Assets/Audio/Fx/hurt.wav");
	return true;
}

bool Item::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	pbody = app->physics->CreateCircle(position.x, position.y, 10, STATIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::ITEM;
	return true;
}

bool Item::Update()
{
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x - (32 / 2));
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y - (32 / 2));
	app->render->DrawTexture(texture, position.x, position.y);
	for (b2Contact* contact = app->physics->world->GetContactList(); contact; contact = contact->GetNext()) {
		if (contact->GetFixtureB()->GetUserData() != NULL)
		{
			//app->font->BlitText(20, 20, 0, "TOUCHING");
			this->pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, -5.0f));
		}
	}
	return true;
}

bool Item::CleanUp()
{
	return true;
}

void Item::OnCollision(PhysBody* physA, PhysBody* physB) 
{

}

void Item::DeathAnimation() 
{
	app->audio->PlayFx(pickUpFx);
}