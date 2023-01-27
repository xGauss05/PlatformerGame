#ifndef __SCENE_MENU_H__
#define __SCENE_MENU_H__

#include "Module.h"
#include "Animation.h"
#include "GuiButton.h"
#include "GuiCheckbox.h"

struct SDL_Texture;

class Scene_Menu : public Module
{
public:

	Scene_Menu();

	// Destructor
	virtual ~Scene_Menu();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool OnGuiMouseClickEvent(GuiControl* control);

public:


private:

	SDL_Texture* background;
	const char* background_texturePath;
	SDL_Texture* settingsBackground;
	SDL_Texture* creditsBackground;

	// Play textures
	SDL_Texture* play_hover;
	SDL_Texture* play_normal;
	SDL_Texture* play_pressed;
	
	// Continue textures
	SDL_Texture* continue_hover;
	SDL_Texture* continue_normal;
	SDL_Texture* continue_pressed;

	// Options textures
	SDL_Texture* options_hover;
	SDL_Texture* options_normal;
	SDL_Texture* options_pressed;

	// Exit textures
	SDL_Texture* exit_hover;
	SDL_Texture* exit_normal;
	SDL_Texture* exit_pressed;

	// Credits textures
	SDL_Texture* credits_hover;
	SDL_Texture* credits_normal;
	SDL_Texture* credits_pressed;

	SDL_Texture* pointer;
	const char* pointer_texturePath; 

	SDL_Texture* exitSettingsTexture;

	bool exit;
	bool menuSettings;
	bool credits;

	Animation pointerArrow;
	Animation* currentPointerAnim = nullptr;

	// Buttons
	// -- Main menu
	GuiButton* playBtn;
	GuiButton* continueBtn;
	GuiButton* menuOptionsBtn;
	GuiButton* creditsBtn;
	GuiButton* menuExitBtn;
	// -- Settings & credits
	GuiButton* returnBtn;

	// Sliders
	// -- Settings
	
	// Checkboxes
	// -- Settings
	GuiCheckbox* fullscreenCbox;
	GuiCheckbox* vsyncCbox;
};

#endif __SCENE_MENU_H__