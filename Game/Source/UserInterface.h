#pragma once
#include "Module.h"
#include "SDL/include/SDL.h"

#include <chrono>
using namespace std::chrono; //(testing)

class UserInterface : public Module
{
public:
	UserInterface();
	virtual ~UserInterface();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void StartTimer(int time);

private:
	bool timerOn;
	milliseconds timerTest = milliseconds(30000);
	milliseconds elapsed;
	high_resolution_clock::time_point startTime;
	high_resolution_clock::time_point currentTime;

	SDL_Texture* lives_tex;
	SDL_Texture* keycard_tex;
};

