#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Textures.h"
#include "Window.h"
#include "Physics.h"
#include "Scene_Menu.h"
#include "Scene_Level1.h"
#include "Scene_Win.h"
#include "Scene_Die.h"
#include "EntityManager.h"
#include "GuiManager.h"
#include "Map.h"
#include "PathFinding.h"
#include "Fonts.h"
#include "FadeToBlack.h"
#include "UserInterface.h"

#include "Defs.h"
#include "Log.h"

Scene_Level1::Scene_Level1() : Module()
{
	name.Create("scene_level1");
}

// Destructor
Scene_Level1::~Scene_Level1()
{

}

// Called before render is available
bool Scene_Level1::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene_Level1");
	bool ret = true;

	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = config.child("player");

	// iterate all objects in the scene
	for (pugi::xml_node enemyNode = config.child("enemy_fly"); enemyNode; enemyNode = enemyNode.next_sibling("enemy_fly"))
	{
		Enemy_Fly* enemy = (Enemy_Fly*)app->entityManager->CreateEntity(EntityType::ENEMY_FLY);
		enemy->parameters = enemyNode;
	}
	for (pugi::xml_node enemyNode = config.child("enemy_walk"); enemyNode; enemyNode = enemyNode.next_sibling("enemy_walk"))
	{
		Enemy_Walk* enemy = (Enemy_Walk*)app->entityManager->CreateEntity(EntityType::ENEMY_WALK);
		enemy->parameters = enemyNode;
	}
	this->active = false;


	for (pugi::xml_node node = config.child("saw").child("pushback"); node; node = node.next_sibling("pushback"))
	{
		sawAnim.PushBack({ node.attribute("x").as_int(),
							node.attribute("y").as_int(),
							node.attribute("width").as_int(),
							node.attribute("height").as_int() });
	}
	saw_texturePath = config.child("saw").attribute("texturepath").as_string();
	sawAnim.loop = config.child("saw").attribute("loop").as_bool();
	sawAnim.speed = config.child("saw").attribute("animspeed").as_float();
	return ret;
}

// Called before the first frame
bool Scene_Level1::Start()
{
	app->physics->Enable();
	font = app->font->Load("Assets/Textures/font.png", " !ª#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[º]^_`abcdefghijklmnopqrstuvwxyz{|}~             ", 6);

	sawTexture = app->tex->Load(saw_texturePath);
	pauseMenuTexture = app->tex->Load("Assets/Textures/menu.png");
	settingsBackground = app->tex->Load("Assets/Textures/settings.png");
	
	bool retLoad = app->map->Load();
	pause = exit = gameplaySettings = false;
	// Create walkability map
	if (retLoad)
	{
		int w, h;
		uchar* data = NULL;

		bool retWalkMap = app->map->CreateWalkabilityMap(w, h, &data);
		if (retWalkMap) app->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);

	}

	uint w, h;
	app->win->GetWindowSize(w, h);
	// Buttons
	// -- Gameplay screen
	pauseBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "PAUSE", { (int)w - 100, (int)20,50,20 }, this);
	// -- Pause screen
	resumeBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "RESUME", { (int)w / 2 - 50,(int)h / 2 - 100,100,20 }, this);
	settingsBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "SETTINGS", { (int)w / 2 - 50,(int)h / 2 - 60,100,20 }, this);
	backToTitleBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "MAIN MENU", { (int)w / 2 - 50,(int)h / 2 - 20,100,20 }, this);
	exitBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "EXIT", { (int)w / 2 - 50,(int)h / 2 + 20,100,20 }, this);
	// -- Settings screen
	gameReturnBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, "X", { (int) w - 55, 5, 50, 50 }, this);

	// Sliders
	// -- Settings screen
	bgmGameSlider = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 7, "BGM slider", { 500, 415, 35, 35 }, this);
	sfxGameSlider = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 8, "SFX slider", { 500, 535, 35, 35 }, this);

	// Checkboxes
	// -- Settings screen
	fullscreenGameCbox = (GuiCheckbox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 9, "Fullscreen cbox", { 550, 755, 50, 50 }, this);
	vsyncGameCbox = (GuiCheckbox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 10, "VSync cbox", { 550, 640, 50, 50 }, this);

	// Initial GUI states
	resumeBtn->state = GuiControlState::DISABLED;
	settingsBtn->state = GuiControlState::DISABLED;
	backToTitleBtn->state = GuiControlState::DISABLED;
	exitBtn->state = GuiControlState::DISABLED;
	pauseBtn->state = GuiControlState::DISABLED;
	gameReturnBtn->state = GuiControlState::DISABLED;
	bgmGameSlider->state = GuiControlState::DISABLED;
	sfxGameSlider->state = GuiControlState::DISABLED;
	fullscreenGameCbox->state = GuiControlState::DISABLED;
	vsyncGameCbox->state = GuiControlState::DISABLED;

	return true;
}

// Called each loop iteration
bool Scene_Level1::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene_Level1::Update(float dt)
{
	if (!app->entityManager->IsEnabled()) app->entityManager->Enable();

	if (!app->ui->IsEnabled()) app->ui->Enable();

	if (app->scene_die->IsEnabled()) app->scene_die->Disable();

	if (app->scene_menu->IsEnabled())
	{
		app->scene_menu->Disable();
	}

	if (IsEnabled() && !app->scene_menu->IsEnabled())
	{
		if (!gameplaySettings) 
		{
			if (pauseBtn->state == GuiControlState::DISABLED) pauseBtn->state = GuiControlState::NORMAL;
		}
	}
	else
	{
		if (pauseBtn->state == GuiControlState::NORMAL) pauseBtn->state = GuiControlState::DISABLED;
		if (resumeBtn->state == GuiControlState::NORMAL) resumeBtn->state = GuiControlState::DISABLED;
		if (settingsBtn->state == GuiControlState::NORMAL) settingsBtn->state = GuiControlState::DISABLED;
		if (backToTitleBtn->state == GuiControlState::NORMAL) backToTitleBtn->state = GuiControlState::DISABLED;
		if (exitBtn->state == GuiControlState::NORMAL) exitBtn->state = GuiControlState::DISABLED;
	}

	// Draw map
	app->map->Draw();

	for (int i = 0; i < saws.Count(); i++)
	{
		sawAnim.Update();
		app->render->DrawTexture(sawTexture,
			METERS_TO_PIXELS(saws.At(i)->data->body->GetPosition().x) - 32,
			METERS_TO_PIXELS(saws.At(i)->data->body->GetPosition().y) - 32,
			&(sawAnim.GetCurrentFrame()));
	}

	if (player->level == 1)
	{
		app->font->BlitText(app->render->camera.x + 130, app->render->camera.y + 580, 0, "Use A and D to move");
		app->font->BlitText(app->render->camera.x + 540, app->render->camera.y + 630, 0, "Press SPACE to jump");
		app->font->BlitText(app->render->camera.x + 1085, app->render->camera.y + 690, 0, "Avoid sharp objects");
	}
	if (player->level == 2)
	{
		app->font->BlitText((app->render->camera.x + 1600) + 405, app->render->camera.y + 610, 0, "Dash into enemies to kill them");
		app->font->BlitText((app->render->camera.x + 1600) + 1150, app->render->camera.y + 650, 0, "Press SPACE while on");
		app->font->BlitText((app->render->camera.x + 1600) + 1135, app->render->camera.y + 670, 0, "a wall to jump off of it");
		app->font->BlitText((app->render->camera.x + 1600) + 820, app->render->camera.y + 300, 0, "Press SPACE mid-air to do a double jump");
	}

	if (pause)
	{
		player->pbody->body->SetActive(false);
		if (app->physics->IsEnabled())
		{
			app->physics->Disable();
		}

		app->render->DrawTexture(pauseMenuTexture, app->render->camera.x * -1 + 450, 200, NULL);
	}
	else
	{
		player->pbody->body->SetActive(true);
		if (!app->physics->IsEnabled())
		{
			app->physics->Enable();
		}

	}

	if (gameplaySettings)
	{
		app->render->DrawTexture(settingsBackground, app->render->camera.x * -1, 0, NULL);
	}

	app->guiManager->Draw();

	if (exit) return false;

	return true;
}

// Called each loop iteration
bool Scene_Level1::PostUpdate()
{
	bool ret = true;

	return ret;
}

// Called before quitting
bool Scene_Level1::CleanUp()
{
	LOG("Freeing Scene_Level1");
	app->tex->UnLoad(sawTexture);
	player = nullptr;
	boxes.Clear();
	saws.Clear();

	app->font->UnLoad(font);

	return true;
}

bool Scene_Level1::OnGuiMouseClickEvent(GuiControl* control)
{
	LOG("Event by %d ", control->id);

	switch (control->id)
	{
	case 1: // Pause btn
		LOG("Pause button click. PAUSE ENABLED.");
		resumeBtn->state = GuiControlState::NORMAL;
		settingsBtn->state = GuiControlState::NORMAL;
		backToTitleBtn->state = GuiControlState::NORMAL;
		exitBtn->state = GuiControlState::NORMAL;
		app->entityManager->Disable();
		pause = true;

		break;
	case 2: // Resume btn
		LOG("Resume button click. PAUSE DISABLED.");
		resumeBtn->state = GuiControlState::DISABLED;
		settingsBtn->state = GuiControlState::DISABLED;
		backToTitleBtn->state = GuiControlState::DISABLED;
		exitBtn->state = GuiControlState::DISABLED;
		app->entityManager->Enable();
		
		pause = false;

		break;
	case 3: // Settings btn
		LOG("Settings button click.");
		bgmGameSlider->SetValue(app->audio->GetBGMVolume());
		sfxGameSlider->SetValue(app->audio->GetSFXVolume());
		gameplaySettings = true;
		pauseBtn->state = GuiControlState::DISABLED;
		resumeBtn->state = GuiControlState::DISABLED;
		settingsBtn->state = GuiControlState::DISABLED;
		backToTitleBtn->state = GuiControlState::DISABLED;
		exitBtn->state = GuiControlState::DISABLED;

		gameReturnBtn->state = GuiControlState::NORMAL;
		bgmGameSlider->state = GuiControlState::NORMAL;
		sfxGameSlider->state = GuiControlState::NORMAL;
		vsyncGameCbox->state = GuiControlState::NORMAL;
		fullscreenGameCbox->state = GuiControlState::NORMAL;

		break;
	case 4: // Main menu btn
		LOG("Main menu button click.");
		app->entityManager->ReviveAllEntities();
		app->entityManager->NeedsToSpawnAllEntities();
		player->ResetGame();
		pause = false;
		app->ftb->SceneFadeToBlack(this, app->scene_menu, 0.0f);
		pauseBtn->state = GuiControlState::DISABLED;
		resumeBtn->state = GuiControlState::DISABLED;
		settingsBtn->state = GuiControlState::DISABLED;
		backToTitleBtn->state = GuiControlState::DISABLED;
		exitBtn->state = GuiControlState::DISABLED;

		break;
	case 5: // Exit btn
		LOG("Exit button click.");
		exit = true;
		break;
	case 6: // Return 
		pauseBtn->state = GuiControlState::NORMAL;
		resumeBtn->state = GuiControlState::NORMAL;
		settingsBtn->state = GuiControlState::NORMAL;
		backToTitleBtn->state = GuiControlState::NORMAL;
		exitBtn->state = GuiControlState::NORMAL;

		gameReturnBtn->state = GuiControlState::DISABLED;
		bgmGameSlider->state = GuiControlState::DISABLED;
		sfxGameSlider->state = GuiControlState::DISABLED;
		vsyncGameCbox->state = GuiControlState::DISABLED;
		fullscreenGameCbox->state = GuiControlState::DISABLED;

		gameplaySettings = false;
		break;
	case 7: // BGM slider
		LOG("BGM slider.");
		app->audio->SetBGMVolume(bgmGameSlider->value);
		break;
	case 8: // SFX slider
		LOG("SFX slider.");
		app->audio->SetSFXVolume(sfxGameSlider->value);
		break;
	case 9: // Fullscreen checkbox
		LOG("Fullscreen checkbox.");
		if (fullscreenGameCbox->state == GuiControlState::SELECTED)
		{
			SDL_SetWindowFullscreen(app->win->window, 0);
		}
		else
		{
			SDL_SetWindowFullscreen(app->win->window, 1);
		}
		break;
	case 10: // VSync checkbox


		break;
	}

	return true;
}

bool Scene_Level1::LoadState(pugi::xml_node& data)
{
	int x = data.child("player").attribute("x").as_int();
	int y = data.child("player").attribute("y").as_int();

	player->pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y)), 0.0f);
	player->level = data.child("player").attribute("level").as_int();
	player->pbody->body->SetLinearVelocity(b2Vec2(data.child("player").attribute("velocity_x").as_float(),
		data.child("player").attribute("velocity_y").as_float()));
	player->dashAvailable = data.child("player").attribute("dashAvailable").as_bool();
	return true;
}

// using append_child and append_attribute
bool Scene_Level1::SaveState(pugi::xml_node& data)
{
	pugi::xml_node node = data.append_child("player");

	node.append_attribute("x") = METERS_TO_PIXELS(player->pbody->body->GetPosition().x);
	node.append_attribute("y") = METERS_TO_PIXELS(player->pbody->body->GetPosition().y);
	node.append_attribute("velocity_x") = player->pbody->body->GetLinearVelocity().x;
	node.append_attribute("velocity_y") = player->pbody->body->GetLinearVelocity().y;
	node.append_attribute("level") = player->level;

	return true;
}
