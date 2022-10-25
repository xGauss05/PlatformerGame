#ifndef __SCENE_MENU_H__
#define __SCENE_MENU_H__

#include "Module.h"

struct SDL_Texture;

class Scene_Menu : public Module
{
public:

	Scene_Menu(bool startEnabled);

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

public:


private:
	SDL_Texture* img;
	SDL_Texture* arrow;
	//int font = 0;
	int choice = 0;
	bool hasSelected = false;
};

#endif __SCENE_MENU_H__