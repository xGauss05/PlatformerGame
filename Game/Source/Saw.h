#ifndef __SAW_H__
#define __SAW_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Saw : public Entity
{
public:

	Saw();

	virtual ~Saw();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	void OnCollision(PhysBody* body);

	void deathAnimation();

public:

private:

	SDL_Texture* texture;
	const char* texturePath;
	PhysBody* pbody;

};

#endif // __SAW_H__