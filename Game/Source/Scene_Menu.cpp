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
#include "UserInterface.h"
#include "GuiManager.h"

#include "Defs.h"
#include "Log.h"

Scene_Menu::Scene_Menu() : Module()
{
	name.Create("scene_menu");
}

// Destructor
Scene_Menu::~Scene_Menu()
{

}

// Called before render is available
bool Scene_Menu::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene_Menu");
	bool ret = true;

	for (pugi::xml_node node = config.child("pointer").child("pushback"); node; node = node.next_sibling("pushback"))
	{
		pointerArrow.PushBack({ node.attribute("x").as_int(),
								node.attribute("y").as_int(),
								node.attribute("width").as_int(),
								node.attribute("height").as_int() });

	}
	pointerArrow.speed = config.child("pointer").attribute("animspeed").as_float();
	currentPointerAnim = &pointerArrow;

	background_texturePath = config.child("background").attribute("texturepath").as_string();
	selector_texturePath = config.child("selector").attribute("texturepath").as_string();
	pointer_texturePath = config.child("pointer").attribute("texturepath").as_string();

	return ret;
}

// Called before the first frame
bool Scene_Menu::Start()
{
	active = false;
	exit = false;
	menuSettings = false;

	background = app->tex->Load(background_texturePath);
	pointer = app->tex->Load(pointer_texturePath);
	selector = app->tex->Load(selector_texturePath);
	settings = app->tex->Load("Assets/Textures/settings.png");
	exitSettingsTexture = app->tex->Load("Assets/Textures/exit.png");
	app->audio->PlayMusic("Assets/Audio/Music/bgm.ogg");

	uint w, h;
	app->win->GetWindowSize(w, h);
	// Buttons
	// -- Main menu
	playBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Play", { 175, 455, 455, 175 }, this);
	//continueBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "Continue", { 175, 665, 455, 175 }, this);
	menuSettingsBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "Settings", { 500, 665, 455, 175 }, this);
	//creditsBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "Credits", { 175, 665, 455, 175 }, this);
	menuExitBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "Exit", { 175, 665, 455, 175 }, this);
	// -- Settings
	settingsExitBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, "Exit Settings", { (int)w-100, 50, 50, 50 }, this);

	playBtn->SetTexture(selector);
	//continueBtn->SetTexture(selector);
	menuSettingsBtn->SetTexture(selector);
	//creditsBtn->SetTexture(selector);
	menuExitBtn->SetTexture(selector);
	settingsExitBtn->SetTexture(exitSettingsTexture);

	testCbox = (GuiCheckbox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 7, "Cbox", { 50, 50, 50, 50 }, this);
	settingsExitBtn->state = GuiControlState::DISABLED;
	testCbox->state = GuiControlState::DISABLED;
	return true;

}

// Called each loop iteration
bool Scene_Menu::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene_Menu::Update(float dt)
{

	if (app->scene->player->level != 1) app->scene->player->level = 1;

	if (app->entityManager->IsEnabled()) app->entityManager->Disable();

	if (this->IsEnabled() && !app->scene->IsEnabled())
	{
		if (!menuSettings) 
		{
			if (playBtn->state == GuiControlState::DISABLED) playBtn->state = GuiControlState::NORMAL;
			//if (continueBtn->state == GuiControlState::DISABLED) continueBtn->state = GuiControlState::NORMAL;
			if (menuSettingsBtn->state == GuiControlState::DISABLED) menuSettingsBtn->state = GuiControlState::NORMAL;
			//if (creditsBtn->state == GuiControlState::DISABLED) creditsBtn->state = GuiControlState::NORMAL;
			if (menuExitBtn->state == GuiControlState::DISABLED) menuExitBtn->state = GuiControlState::NORMAL;
		}
	}
	else
	{
		if (playBtn->state != GuiControlState::DISABLED) playBtn->state = GuiControlState::DISABLED;
		//if (continueBtn->state != GuiControlState::DISABLED) continueBtn->state = GuiControlState::DISABLED;
		if (menuSettingsBtn->state != GuiControlState::DISABLED) menuSettingsBtn->state = GuiControlState::DISABLED;
		//if (creditsBtn->state != GuiControlState::DISABLED) creditsBtn->state = GuiControlState::DISABLED;
		if (menuExitBtn->state != GuiControlState::DISABLED) menuExitBtn->state = GuiControlState::DISABLED;
	}

	if (app->scene->IsEnabled()) app->scene->Disable();

	if (app->scene_die->IsEnabled()) app->scene_die->Disable();

	if (app->ui->IsEnabled()) app->ui->Disable();

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) return false;

	if (exit) return false;

	return true;
}

// Called each loop iteration
bool Scene_Menu::PostUpdate()
{
	bool ret = true;
	SDL_Rect rect;
	rect.h = 20;
	rect.w = 20;

	app->render->DrawTexture(background, app->render->camera.x * -1, app->render->camera.y, NULL);
	if (menuSettings)
	{
		app->render->DrawTexture(settings, app->render->camera.x * -1, app->render->camera.y, NULL);
	}
	int a, b;
	app->input->GetMousePosition(a, b);

	rect = currentPointerAnim->GetCurrentFrame();
	currentPointerAnim->Update();
	app->render->DrawTexture(pointer, a, b, &rect);

	app->guiManager->Draw();
	return ret;
}

// Called before quitting
bool Scene_Menu::CleanUp()
{
	LOG("Freeing Scene_Menu");
	pointer = nullptr;
	delete pointer;
	background = nullptr;
	delete background;

	return true;
}

bool Scene_Menu::OnGuiMouseClickEvent(GuiControl* control)
{
	LOG("Event by %d ", control->id);

	switch (control->id)
	{
	case 1: // Play btn
		LOG("Play button click.");
		app->entityManager->ActivateEnemies();
		app->ftb->SceneFadeToBlack(this, app->scene, 0.0f);
		app->ui->StartTimer(30000);
		playBtn->state = GuiControlState::DISABLED;
		//continueBtn->state = GuiControlState::DISABLED;
		menuSettingsBtn->state = GuiControlState::DISABLED;
		//creditsBtn->state = GuiControlState::DISABLED;
		menuExitBtn->state = GuiControlState::DISABLED;

		break;
	case 2: // Continue btn

		// do behavior
		playBtn->state = GuiControlState::DISABLED;
		//continueBtn->state = GuiControlState::DISABLED;
		menuSettingsBtn->state = GuiControlState::DISABLED;
		//creditsBtn->state = GuiControlState::DISABLED;
		menuExitBtn->state = GuiControlState::DISABLED;
		break;
	case 3: // Settings btn
		LOG("Settings button click.");
		menuSettings = true;
		playBtn->state = GuiControlState::DISABLED;
		//continueBtn->state = GuiControlState::DISABLED;
		menuSettingsBtn->state = GuiControlState::DISABLED;
		//creditsBtn->state = GuiControlState::DISABLED;
		menuExitBtn->state = GuiControlState::DISABLED;
		testCbox->state = GuiControlState::NORMAL;
		settingsExitBtn->state = GuiControlState::NORMAL;

		break;
	case 4: // Credits btn
		LOG("Main menu button click.");
		// do behavior

		break;
	case 5: // Exit btn
		LOG("Exit button.");
		exit = true;

		break;
	case 6: // Exit settings btn
		LOG("Exit settings button.");
		menuSettings = false;
		playBtn->state = GuiControlState::NORMAL;
		//continueBtn->state = GuiControlState::NORMAL;
		menuSettingsBtn->state = GuiControlState::NORMAL;
		//creditsBtn->state = GuiControlState::NORMAL;
		menuExitBtn->state = GuiControlState::NORMAL;
		testCbox->state = GuiControlState::DISABLED;
		settingsExitBtn->state = GuiControlState::DISABLED;
		break;
	case 7: // Test cbox
		LOG("Testing checkbox.");
		break;

	}

	return true;
}