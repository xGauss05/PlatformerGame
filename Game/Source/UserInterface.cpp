#include "App.h"
#include "UserInterface.h"
#include "Fonts.h"

UserInterface::UserInterface() : Module()
{
	name.Create("UI");
}

UserInterface::~UserInterface() {}

bool UserInterface::Awake(pugi::xml_node&)
{
	this->active = false;
	return true;
}

bool UserInterface::Start()
{
	return true;
}

bool UserInterface::PreUpdate()
{
	return true;
}

bool UserInterface::Update(float dt)
{
	return true;
}

bool UserInterface::PostUpdate()
{
	app->font->BlitText(500, 100, 0, "Hola que tal this is UI");
	return true;
}

bool UserInterface::CleanUp()
{
	return true;
}
