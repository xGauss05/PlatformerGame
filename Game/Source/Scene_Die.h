#ifndef __SCENE_DIE_H__
#define __SCENE_DIE_H__

#include "Module.h"
#include "Animation.h"

struct SDL_Texture;

class Scene_Die : public Module
{
public:

	Scene_Die();

	// Destructor
	virtual ~Scene_Die();

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
	iPoint oldposition;
	SDL_Texture* background;
	const char* background_texturePath;
	SDL_Texture* arrow;
	SDL_Texture* pointer;
	const char* pointer_texturePath;
	//int font = 0;
	int choice = 0;
	bool hasSelected = false;
	bool hasRecovered = false;
	Animation anim;
	Animation* currentAnim = nullptr;
	Animation pointerArrow;
	Animation* currentPointerAnim = nullptr;

};

#endif __SCENE_DIE_H__