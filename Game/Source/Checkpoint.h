#ifndef __CHECKPOINT_H__
#define __CHECKPOINT_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Checkpoint : public Entity
{
public:

	Checkpoint();

	virtual ~Checkpoint();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void DeathAnimation();

	void SaveProgress();

public:

private:

	SDL_Texture* checkpointTexture;
	PhysBody* pbody;
	bool reached;
	int checkpointSfx;
};

#endif // __CHECKPOINT_H__