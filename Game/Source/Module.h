#ifndef __MODULE_H__
#define __MODULE_H__

#include "SString.h"

#include "PugiXml/src/pugixml.hpp"

class App;
class PhysBody;

class Module
{
public:

	Module(bool startEnabled) : active(startEnabled)
	{}

	void Init()
	{}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool LoadState(pugi::xml_node&)
	{
		return true;
	}

	virtual bool SaveState(pugi::xml_node&)
	{
		return true;
	}

	virtual void OnCollision(PhysBody* bodyA, PhysBody* bodyB)
	{
	}

	// Switches isEnabled and calls Start() method
	virtual void Enable() {
		if (!active) {
			active = true;
			Start();
		}
	}

	// Switches isEnabled and calls CleanUp() method
	virtual void Disable() {
		if (active) {
			active = false;
			CleanUp();
		}
	}

	inline bool IsEnabled() const {
		return active;
	}

public:

	SString name;
	bool active = true;
private:

};

#endif // __MODULE_H__