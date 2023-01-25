#ifndef __GUIBUTTON_H__
#define __GUIBUTTON_H__

#include "GuiControl.h"

#include "Point.h"
#include "SString.h"

enum class ButtonType 
{
	PLAY,
	CONTINUE,
	SETTINGS,
	CREDITS,
	EXIT,
	NONE
};

class GuiButton : public GuiControl
{
public:

	GuiButton(uint32 id, SDL_Rect bounds, const char* text);
	virtual ~GuiButton();

	bool Update(float dt);
	bool Draw(Render* render);

	int mouseX, mouseY;
	unsigned int click;

	bool canClick = true;
	bool drawBasic = false;

	ButtonType btnType;

	uint selectFx, hoverFx;
};

#endif // __GUIBUTTON_H__