#include "Checkpoint.h"
#include "Textures.h"
#include "Physics.h"
#include "App.h"
#include "Scene_Level1.h"
#include "Audio.h"
#include "Point.h"
#include "Input.h"

Checkpoint::Checkpoint() : Entity(EntityType::CHECKPOINT)
{
	name.Create("checkpoint");
}

Checkpoint::~Checkpoint()
{

}

bool Checkpoint::Awake()
{
	spawn.x = position.x;
	spawn.y = position.y;

	return true;
}

bool Checkpoint::Start()
{
	checkpointTexture = app->tex->Load("Assets/Textures/checkpoint.png");
	pbody = app->physics->CreateRectangleSensor(position.x + 16, position.y + 32, 32, 64, STATIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::CHECKPOINT;
	checkpointSfx = app->audio->LoadFx("Assets/Audio/Fx/checkpoint.wav");
	reached = false;

	return true;
}

bool Checkpoint::Update()
{
	if (reached)
	{
		SDL_Rect rect;
		rect = { 32,0,32,64 };
		app->render->DrawTexture(checkpointTexture,
			METERS_TO_PIXELS(pbody->body->GetPosition().x) - 16,
			METERS_TO_PIXELS(pbody->body->GetPosition().y) - 32,
			&rect);
	}
	else
	{
		SDL_Rect rect;
		rect = { 0,0,32,64 };
		app->render->DrawTexture(checkpointTexture,
			METERS_TO_PIXELS(pbody->body->GetPosition().x) - 16,
			METERS_TO_PIXELS(pbody->body->GetPosition().y) - 32,
			&rect);
	}
	

	return true;
}

bool Checkpoint::CleanUp()
{
	app->tex->UnLoad(checkpointTexture);
	return true;
}

void Checkpoint::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (physB->ctype == ColliderType::PLAYER)
	{
		if (!app->scene->player->checkpointReached) 
		{
			app->audio->PlayFx(checkpointSfx);
		}
		reached = true;
		app->scene->player->checkpointReached = true;
		SaveProgress();
		app->scene->player->ReadSpawn();
		
	}
}

void Checkpoint::DeathAnimation()
{

}

void Checkpoint::SaveProgress()
{
	pugi::xml_document* saveDoc = new pugi::xml_document();
	pugi::xml_node saveStateNode = saveDoc->append_child("save_state");

	pugi::xml_node levelNode = saveStateNode.append_child("level"); 
	levelNode.append_attribute("value") = this->level;

	pugi::xml_node spawnNode = saveStateNode.append_child("spawnPos");
	spawnNode.append_attribute("x") = METERS_TO_PIXELS(this->pbody->body->GetPosition().x);
	spawnNode.append_attribute("y") = METERS_TO_PIXELS(this->pbody->body->GetPosition().y);

	saveDoc->save_file("save_game.xml");
}