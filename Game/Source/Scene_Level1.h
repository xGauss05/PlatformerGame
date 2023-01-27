#ifndef __SCENE_LEVEL1_H__
#define __SCENE_LEVEL1_H__

#include "Module.h"
#include "Player.h"
#include "Enemy_Fly.h"
#include "Enemy_Walk.h"
#include "Animation.h"
#include "GuiButton.h"
#include "GuiCheckbox.h"
#include "GuiSlider.h"

struct SDL_Texture;

class Scene_Level1 : public Module
{
public:

	Scene_Level1();

	// Destructor
	virtual ~Scene_Level1();

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

	bool LoadState(pugi::xml_node& data);

	bool SaveState(pugi::xml_node& data);

public:

	Player* player;
	List<PhysBody*> boxes;
	List<PhysBody*> saws;
	List<PhysBody*> checkpoints;

	bool pause;

private:

	SDL_Texture* sawTexture;
	SDL_Texture* pauseMenuTexture;
	SDL_Texture* settingsBackground;
	
	const char* saw_texturePath;
	bool exit;

	Animation sawAnim;
	int font = 0;

	bool gameplaySettings;

	// Buttons
	// -- Gameplay menu
	GuiButton* pauseBtn;
	GuiButton* resumeBtn;
	GuiButton* backToTitleBtn;
	GuiButton* settingsBtn;
	GuiButton* exitBtn;
	// -- Settings
	GuiButton* gameReturnBtn;

	// Sliders
	// -- Settings
	GuiSlider* bgmGameSlider;
	GuiSlider* sfxGameSlider;

	// Checkboxes
	// -- Settings
	GuiCheckbox* fullscreenGameCbox;
	GuiCheckbox* vsyncGameCbox;
};

#endif __SCENE_LEVEL1_H__
