#ifndef ___ENEMY_FLY_H__
#define ___ENEMY_FLY_H__

#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Enemy_Fly : public Entity
{
public:

	Enemy_Fly();
	virtual ~Enemy_Fly();

	void InitAnims();

	bool Awake();

	bool Start();

	void TeleportTo(iPoint position);

	bool Update();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void DeathAnimation();

public:

	int dieFx;

private:

	SDL_Texture* texture;
	const char* texturePath;

	Animation* currentAnim = nullptr;
	Animation leftMove, rightMove;

	float speedCap = 6.0f;

	DynArray<iPoint> pathToPlayer;

	bool pendingToDelete = false;
};

#endif // ___ENEMY_FLY_H__