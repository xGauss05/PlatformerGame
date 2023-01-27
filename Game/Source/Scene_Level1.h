#ifndef __SCENE_LEVEL1_H__
#define __SCENE_LEVEL1_H__

#include "Module.h"
#include "Player.h"
#include "Enemy_Fly.h"
#include "Enemy_Walk.h"
#include "Animation.h"
#include "GuiButton.h"

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

private:

	SDL_Texture* sawTexture;
	SDL_Texture* pauseMenuTexture;
	const char* saw_texturePath;
	bool pause, exit;

	Animation sawAnim;
	int font = 0;

	// Buttons
	GuiButton* pauseBtn;
	GuiButton* resumeBtn;
	GuiButton* backToTitleBtn;
	GuiButton* settingsBtn;
	GuiButton* exitBtn;

};

#endif __SCENE_LEVEL1_H__
