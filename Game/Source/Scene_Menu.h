#ifndef __SCENE_MENU_H__
#define __SCENE_MENU_H__

#include "Module.h"
#include "Animation.h"

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

public:
	bool hasSelected = false;

private:

	SDL_Texture* background;
	const char* background_texturePath;

	SDL_Texture* selector;
	const char* selector_texturePath;

	SDL_Texture* pointer;
	const char* pointer_texturePath; 

	int selectedFx;
	int choice = 0;
	
	Animation pointerArrow;
	Animation* currentPointerAnim = nullptr;

};

#endif __SCENE_MENU_H__