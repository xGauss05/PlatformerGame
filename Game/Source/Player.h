#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "Debug.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	void InitAnims();

	bool Awake();

	bool Start();

	void AnimationLogic();

	void MovementLogic();

	void NormalsCheck();
	
	void SetSpawn(iPoint position, iPoint cameraPosition);

	void TeleportTo(iPoint position);

	void LevelSelector();

	void Reset();

	bool Update();

	bool CleanUp();

	void OnCollision(PhysBody* bodyA, PhysBody* physB);

	void DeathAnimation();

public:

	int maxJumps;
	int currentJumps;
	int level = 1;
	bool isDead = false;
	bool doorReached = false;

	// SFX indexes
	int dieFx, landingFx, jumpFx,
		goalFx;

	iPoint spawn;

	PhysBody* pbody;

private:

	SDL_Texture* texture;
	const char* texturePath;
	int width = 64, height = 88;

	Animation* currentAnim = nullptr;

	Animation rightIdle, rightRun, rightJump,
			  rightFall, rightWall;

	Animation leftIdle, leftRun, leftJump,
			  leftFall, leftWall;
	
	//Movement variables
	float movementForce = 10.0f;
	float jumpForce = 300.f;
	float movementDampen = 20.0f;
	float speedCap = 12.0;
	float idleDampenMultiplier = 3.0f;

	float normal_x = 0.0f, normal_y = 0.0f;

	friend void Debug::DebugDraw();
};

#endif // __PLAYER_H__