#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	void initAnims();

	bool Awake();

	bool Start();

	//bool PreUpdate();

	bool Update();

	bool CleanUp();

	void OnCollision(PhysBody* body);

	void deathAnimation();

public:
	bool isDead = false;
	int maxJumps;
	int currentJumps;
	int level = 1;
private:

	SDL_Texture* texture;
	const char* texturePath;
	int width = 64, height = 88;

	Animation* currentAnim = nullptr;

	Animation rightIdle;
	Animation rightRun;
	Animation rightJump;
	Animation rightFall;

	Animation leftIdle;
	Animation leftRun;
	Animation leftJump;
	Animation leftFall;

	PhysBody* pbody;

	int lifes = 2;
	int dieFx;

	//Movement variables
	float movementForce = 10.0f;
	float jumpForce = 300.f;
	float movementDampen = 20.0f;
	float speedCap = 12.0;
	float idleDampenMultiplier = 3.0f;

	float normal_x = 0.0f, normal_y = 0.0f;
};

#endif // __PLAYER_H__