#include "SceneTransition.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene_Menu.h"
#include "Scene_Level1.h"
#include "Scene_Die.h"
#include "EntityManager.h"
#include "Map.h"
#include "Fonts.h"
#include "FadeToBlack.h"
#include "Physics.h"

#include "Defs.h"
#include "Log.h"

Scene_Transition::Scene_Transition() : Module()
{
	name.Create("scene_transition");
}

// Destructor
Scene_Transition::~Scene_Transition() {}

// Called before render is available
bool Scene_Transition::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene_Transition");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene_Transition::Start()
{
	active = false;
	return true;
}

// Called each loop iteration
bool Scene_Transition::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene_Transition::Update(float dt)
{
	if (app->entityManager->IsEnabled()) {
		app->entityManager->Disable();
	}

	app->ftb->SceneFadeToBlack(this, app->scene, 25.0f);

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		return false;

	return true;
}

// Called each loop iteration
bool Scene_Transition::PostUpdate()
{
	bool ret = true;

	return ret;
}

// Called before quitting
bool Scene_Transition::CleanUp()
{
	LOG("Freeing Scene_Menu");
	return true;
}
