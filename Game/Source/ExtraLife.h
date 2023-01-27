#ifndef __EXTRALIFE_H__
#define __EXTRALIFE_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class ExtraLife : public Entity
{
public:

	ExtraLife();

	virtual ~ExtraLife();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void DeathAnimation();

public:

private:

	SDL_Texture* texture;
	PhysBody* pbody;

};

#endif // __EXTRALIFE_H__