#ifndef __GUISLIDER_H__
#define __GUISLIDER_H__

#include "GuiControl.h"

#include "Point.h"
#include "SString.h"

class GuiSlider : public GuiControl
{
public:

	GuiSlider(uint32 id, SDL_Rect bounds, const char* text);
	virtual ~GuiSlider();

	bool Update(float dt);
	bool Draw(Render* render);
	void SetValue(int value);

	int mouseX, mouseY, mousePrevX, mousePrevY;
	
	unsigned int click;

	bool canClick = true;
	bool drawBasic = false;

	uint selectFx, hoverFx;

	int length, value;

	SDL_Rect sliderLine,buttonBox;

};

#endif // __GUISLIDER_H__
