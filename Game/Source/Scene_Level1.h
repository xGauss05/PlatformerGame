#ifndef __SCENE_LEVEL1_H__
#define __SCENE_LEVEL1_H__

#include "Module.h"
#include "Player.h"
#include "Item.h"
#include "Animation.h"

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

	bool LoadState(pugi::xml_node& data);
	bool SaveState(pugi::xml_node& data);

public:

	Player* player;
	List<PhysBody*> boxes;
	List<PhysBody*> saws;

private:

	SDL_Texture* sawTexture;
	Animation sawAnim;
	int font = 0;

};

#endif __SCENE_LEVEL1_H__
