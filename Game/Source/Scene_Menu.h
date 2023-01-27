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

	SDL_Texture* selector;
	const char* selector_texturePath;

	SDL_Texture* pointer;
	const char* pointer_texturePath; 

	SDL_Texture* settings;
	SDL_Texture* exitSettingsTexture;

	bool exit;
	bool menuSettings;

	Animation pointerArrow;
	Animation* currentPointerAnim = nullptr;

	// Buttons
	// -- Main menu
	GuiButton* playBtn;
	GuiButton* continueBtn;
	GuiButton* menuSettingsBtn;
	GuiButton* creditsBtn;
	GuiButton* menuExitBtn;
	// -- Settings
	GuiButton* settingsExitBtn;

	// Sliders
	// -- Settings
	
	// Checkboxes
	// -- Settings
	GuiCheckbox* testCbox;
};

#endif __SCENE_MENU_H__