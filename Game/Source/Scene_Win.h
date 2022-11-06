#ifndef __SCENE_WIN_H__
#define __SCENE_WIN_H__

#include "Module.h"
#include "Animation.h"

#include <chrono>
using namespace std::chrono;

struct SDL_Texture;

class Scene_Win : public Module
{
public:

	Scene_Win();

	// Destructor
	virtual ~Scene_Win();

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

	SDL_Texture* background;
	const char* background_texturePath;

	high_resolution_clock::time_point start;
	high_resolution_clock::time_point currentTime;
	milliseconds elapsed;

};

#endif __SCENE_WIN_H__
