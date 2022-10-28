#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	void deathAnimation();

public:
	bool isDead = false;;
private:

	SDL_Texture* texture;
	const char* texturePath;
	int width = 32, height = 32;
	PhysBody* pbody;
	int lifes = 2;
	int dieFx;

	//Movement variables
	float movementForce = 10.0f;
	float jumpForce = 300.f;
	float movementDampen = 20.0f;
	float speedCap = 12.0;
	float idleDampenMultiplier = 3.0f;
};

#endif // __PLAYER_H__