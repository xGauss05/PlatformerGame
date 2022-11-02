#ifndef __SCENE_LEVEL1_H__
#define __SCENE_LEVEL1_H__

#include "Module.h"
#include "Player.h"
#include "Item.h"

struct SDL_Texture;

class Scene_Level1 : public Module
{
public:

	Scene_Level1(bool startEnabled);

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

	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:

	Player* player;
	List<PhysBody*> boxes;

private:
	SDL_Texture* img;
	int font = 0;

};

#endif __SCENE_LEVEL1_H__