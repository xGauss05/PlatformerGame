#pragma once
#include "Module.h"

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
};

