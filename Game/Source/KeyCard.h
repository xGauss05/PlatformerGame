#ifndef __KEYCARD_H__
#define __KEYCARD_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class KeyCard : public Entity
{
public:

	KeyCard();

	virtual ~KeyCard();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void DeathAnimation();

public:

private:

	SDL_Texture* texture;
	const char* texturePath;
	PhysBody* pbody;
	int pickUpFx;
};

#endif // __KEYCARD_H__