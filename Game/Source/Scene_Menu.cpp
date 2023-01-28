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
	pointer_texturePath = config.child("pointer").attribute("texturepath").as_string();

	return ret;
}

// Called before the first frame
bool Scene_Menu::Start()
{
	active = false;
	exit = false;
	menuSettings = false;
	credits = false;

	background = app->tex->Load(background_texturePath);
	settingsBackground = app->tex->Load("Assets/Textures/settings.png");
	creditsBackground = app->tex->Load("Assets/Textures/credits.png");

	pointer = app->tex->Load(pointer_texturePath);
	play_hover = app->tex->Load("Assets/Textures/Buttons/Play_Hover.png");
	play_normal = app->tex->Load("Assets/Textures/Buttons/Play_Normal.png");
	play_pressed = app->tex->Load("Assets/Textures/Buttons/Play_Pressed.png");

	continue_hover = app->tex->Load("Assets/Textures/Buttons/Continue_Hover.png");
	continue_normal = app->tex->Load("Assets/Textures/Buttons/Continue_Normal.png");
	continue_pressed = app->tex->Load("Assets/Textures/Buttons/Continue_Pressed.png");

	credits_hover = app->tex->Load("Assets/Textures/Buttons/Credits_Hover.png");
	credits_normal = app->tex->Load("Assets/Textures/Buttons/Credits_Normal.png");
	credits_pressed = app->tex->Load("Assets/Textures/Buttons/Credits_Pressed.png");

	exit_hover = app->tex->Load("Assets/Textures/Buttons/Exit_Hover.png");
	exit_normal = app->tex->Load("Assets/Textures/Buttons/Exit_Normal.png");
	exit_pressed = app->tex->Load("Assets/Textures/Buttons/Exit_Pressed.png");

	options_hover = app->tex->Load("Assets/Textures/Buttons/Options_Hover.png");
	options_normal = app->tex->Load("Assets/Textures/Buttons/Options_Normal.png");
	options_pressed = app->tex->Load("Assets/Textures/Buttons/Options_Pressed.png");

	exitSettingsTexture = app->tex->Load("Assets/Textures/exit.png");
	app->audio->PlayMusic("Assets/Audio/Music/bgm.ogg");

	uint w, h;
	app->win->GetWindowSize(w, h);
	// Buttons
	// -- Main menu
	playBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Play", { 50, 350, 513, 155 }, this);
	continueBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "Continue", { 600, 350, 513, 155 }, this);
	menuOptionsBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "Options", { 50, 510, 513, 155 }, this);
	creditsBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "Credits", { 600, 665, 513, 155 }, this);
	menuExitBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "Exit", { 50, 670, 513, 155 }, this);
	// -- Settings
	returnBtn = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, "X", { (int)w - 55, 5, 50, 50 }, this);

	// Button textures
	playBtn->SetTexture(play_normal);
	playBtn->SetFocusedTexture(play_hover);
	playBtn->SetPressedTexture(play_pressed);

	continueBtn->SetTexture(continue_normal);
	continueBtn->SetFocusedTexture(continue_hover);
	continueBtn->SetPressedTexture(continue_pressed);

	menuOptionsBtn->SetTexture(options_normal);
	menuOptionsBtn->SetFocusedTexture(options_hover);
	menuOptionsBtn->SetPressedTexture(options_pressed);

	creditsBtn->SetTexture(credits_normal);
	creditsBtn->SetFocusedTexture(credits_hover);
	creditsBtn->SetPressedTexture(credits_pressed);

	menuExitBtn->SetTexture(exit_normal);
	menuExitBtn->SetFocusedTexture(exit_hover);
	menuExitBtn->SetPressedTexture(exit_pressed);

	// Checkboxes
	// -- Settings
	fullscreenCbox = (GuiCheckbox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 7, "Fullscreen cbox", { 550, 755, 50, 50 }, this);
	vsyncCbox = (GuiCheckbox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 8, "VSync cbox", { 550, 640, 50, 50 }, this);


	// Sliders
	// -- Settings
	bgmSlider = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 9, "BGM slider", { 500, 415, 35, 35 }, this);
	sfxSlider = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 10, "SFX slider", { 500, 535, 35, 35 }, this);

	// Initial GUI states
	continueBtn->state = GuiControlState::DISABLED;
	returnBtn->state = GuiControlState::DISABLED;
	fullscreenCbox->state = GuiControlState::DISABLED;
	vsyncCbox->state = GuiControlState::DISABLED;
	bgmSlider->state = GuiControlState::DISABLED;
	sfxSlider->state = GuiControlState::DISABLED;

	bgmSlider->SetValue(app->audio->GetBGMVolume());
	sfxSlider->SetValue(app->audio->GetSFXVolume());
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

	if (IsEnabled() && !app->scene->IsEnabled())
	{
		if (!menuSettings && !credits)
		{
			if (playBtn->state == GuiControlState::DISABLED) playBtn->state = GuiControlState::NORMAL;
			pugi::xml_document gameStateFile;
			pugi::xml_parse_result result = gameStateFile.load_file("save_game.xml");


			if (result)
			{
				if (continueBtn->state == GuiControlState::DISABLED) continueBtn->state = GuiControlState::NORMAL;
			}
			if (menuOptionsBtn->state == GuiControlState::DISABLED) menuOptionsBtn->state = GuiControlState::NORMAL;
			if (creditsBtn->state == GuiControlState::DISABLED) creditsBtn->state = GuiControlState::NORMAL;
			if (menuExitBtn->state == GuiControlState::DISABLED) menuExitBtn->state = GuiControlState::NORMAL;
		}
	}
	else
	{
		if (playBtn->state != GuiControlState::DISABLED) playBtn->state = GuiControlState::DISABLED;
		if (continueBtn->state != GuiControlState::DISABLED) continueBtn->state = GuiControlState::DISABLED;
		if (menuOptionsBtn->state != GuiControlState::DISABLED) menuOptionsBtn->state = GuiControlState::DISABLED;
		if (creditsBtn->state != GuiControlState::DISABLED) creditsBtn->state = GuiControlState::DISABLED;
		if (menuExitBtn->state != GuiControlState::DISABLED) menuExitBtn->state = GuiControlState::DISABLED;
		if (returnBtn->state != GuiControlState::DISABLED) playBtn->state = GuiControlState::DISABLED;
		if (fullscreenCbox->state != GuiControlState::DISABLED) continueBtn->state = GuiControlState::DISABLED;
		if (vsyncCbox->state != GuiControlState::DISABLED) continueBtn->state = GuiControlState::DISABLED;

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
		app->render->DrawTexture(settingsBackground, app->render->camera.x * -1, app->render->camera.y, NULL);
	}

	if (credits)
	{
		app->render->DrawTexture(creditsBackground, app->render->camera.x * -1, app->render->camera.y, NULL);
	}

	int a, b;
	app->input->GetMousePosition(a, b);

	rect = currentPointerAnim->GetCurrentFrame();
	currentPointerAnim->Update();

	app->guiManager->Draw();
	app->render->DrawTexture(pointer, a, b, &rect);
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
	settingsBackground = nullptr;
	delete settingsBackground;
	creditsBackground = nullptr;
	delete creditsBackground;
	play_hover = nullptr;
	delete play_hover;
	play_normal = nullptr;
	delete play_normal;
	play_pressed = nullptr;
	delete play_pressed;
	continue_hover = nullptr;
	delete continue_hover;
	continue_normal = nullptr;
	delete continue_normal;
	continue_pressed = nullptr;
	delete continue_pressed;
	options_hover = nullptr;
	delete options_hover;
	options_normal = nullptr;
	delete options_normal;
	options_pressed = nullptr;
	delete options_pressed;
	exit_hover = nullptr;
	delete exit_hover;
	exit_normal = nullptr;
	delete exit_normal;
	exit_pressed = nullptr;
	delete exit_pressed;
	credits_hover = nullptr;
	delete credits_hover;
	credits_normal = nullptr;
	delete credits_normal;
	credits_pressed = nullptr;
	delete credits_pressed;
	exitSettingsTexture = nullptr;
	delete exitSettingsTexture;

	return true;
}

bool Scene_Menu::OnGuiMouseClickEvent(GuiControl* control)
{
	LOG("Event by %d ", control->id);
	int flags = SDL_GetWindowFlags(app->win->window);
	switch (control->id)
	{
	case 1: // Play btn
		LOG("Play button.");
		app->entityManager->ActivateEnemies();
		app->ftb->SceneFadeToBlack(this, app->scene, 0.0f);
		app->ui->StartTimer(30000);
		playBtn->state = GuiControlState::DISABLED;
		continueBtn->state = GuiControlState::DISABLED;
		menuOptionsBtn->state = GuiControlState::DISABLED;
		creditsBtn->state = GuiControlState::DISABLED;
		menuExitBtn->state = GuiControlState::DISABLED;

		break;
	case 2: // Continue btn
		LOG("Continue button.");
		app->entityManager->ActivateEnemies();
		app->ftb->SceneFadeToBlack(this, app->scene, 0.0f);
		app->ui->StartTimer(30000);
		app->LoadGameRequest();
		playBtn->state = GuiControlState::DISABLED;
		continueBtn->state = GuiControlState::DISABLED;
		menuOptionsBtn->state = GuiControlState::DISABLED;
		creditsBtn->state = GuiControlState::DISABLED;
		menuExitBtn->state = GuiControlState::DISABLED;
		break;
	case 3: // Settings btn
		LOG("Settings button.");
		menuSettings = true;
		bgmSlider->SetValue(app->audio->GetBGMVolume());
		sfxSlider->SetValue(app->audio->GetSFXVolume());
		playBtn->state = GuiControlState::DISABLED;
		continueBtn->state = GuiControlState::DISABLED;
		menuOptionsBtn->state = GuiControlState::DISABLED;
		creditsBtn->state = GuiControlState::DISABLED;
		menuExitBtn->state = GuiControlState::DISABLED;
		
		(flags & SDL_WINDOW_FULLSCREEN) ? fullscreenCbox->state = GuiControlState::SELECTED
										: fullscreenCbox->state = GuiControlState::NORMAL;
		
		(app->vsync) ? vsyncCbox->state = GuiControlState::SELECTED
					 : vsyncCbox->state = GuiControlState::NORMAL;

		returnBtn->state = GuiControlState::NORMAL;
		bgmSlider->state = GuiControlState::NORMAL;
		sfxSlider->state = GuiControlState::NORMAL;

		break;
	case 4: // Credits btn
		LOG("Credits button.");
		credits = true;
		playBtn->state = GuiControlState::DISABLED;
		continueBtn->state = GuiControlState::DISABLED;
		menuOptionsBtn->state = GuiControlState::DISABLED;
		creditsBtn->state = GuiControlState::DISABLED;
		menuExitBtn->state = GuiControlState::DISABLED;

		returnBtn->state = GuiControlState::NORMAL;
		break;
	case 5: // Exit btn
		LOG("Exit button.");
		exit = true;

		break;
	case 6: // Return btn
		LOG("Return button.");
		if (menuSettings) menuSettings = false;
		if (credits) credits = false;
		playBtn->state = GuiControlState::NORMAL;
		continueBtn->state = GuiControlState::NORMAL;
		menuOptionsBtn->state = GuiControlState::NORMAL;
		creditsBtn->state = GuiControlState::NORMAL;
		menuExitBtn->state = GuiControlState::NORMAL;
		fullscreenCbox->state = GuiControlState::DISABLED;
		vsyncCbox->state = GuiControlState::DISABLED;
		returnBtn->state = GuiControlState::DISABLED;
		bgmSlider->state = GuiControlState::DISABLED;
		sfxSlider->state = GuiControlState::DISABLED;
		break;
	case 7: // Fullscreen cbox
		LOG("Fullscreen checkbox.");
		(fullscreenCbox->state == GuiControlState::SELECTED) ? SDL_SetWindowFullscreen(app->win->window, 0)
															 : SDL_SetWindowFullscreen(app->win->window, 1);
		
		break;

	case 8: // VSync cbox
		LOG("VSync checkbox.");
		app->vsync = !app->vsync;
		break;
	case 9: // BGM slider
		LOG("BGM slider.");
		app->audio->SetBGMVolume(bgmSlider->value);
		break;
	case 10: // SFX slider
		LOG("SFX slider.");
		app->audio->SetSFXVolume(sfxSlider->value);
		break;
	}


	return true;
}

