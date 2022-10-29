#ifndef __ITEM_H__
#define __ITEM_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Item : public Entity
{
public:

	Item();
	virtual ~Item();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	void OnCollision(PhysBody* body);

	void deathAnimation();

public:

	bool isPicked = false;

private:

	int pickUpFx;
	SDL_Texture* texture;
	const char* texturePath;
	PhysBody* pbody;

};

#endif // __ITEM_H__