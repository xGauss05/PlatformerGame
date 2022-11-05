#ifndef __SCENE_TRANSITION_H__
#define __SCENE_TRANSITION_H__

#include "Module.h"
#include "Animation.h"

struct SDL_Texture;

class Scene_Transition : public Module
{
public:

	Scene_Transition();

	// Destructor
	virtual ~Scene_Transition();

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

};

#endif __SCENE_TRANSITION_H__