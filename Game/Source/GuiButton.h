#ifndef __GUIBUTTON_H__
#define __GUIBUTTON_H__

#include "GuiControl.h"

#include "Point.h"
#include "SString.h"

class GuiButton : public GuiControl 
{
public:

	GuiButton(uint32 id, SDL_Rect bounds, const char* text);
	virtual ~GuiButton();

	bool Update(float dt);
	bool Draw(Render* render);

	void SetPressedTexture(SDL_Texture* tex);
	void SetFocusedTexture(SDL_Texture* tex);

	int mouseX, mouseY;
	unsigned int click;

	bool canClick = true;
	bool drawBasic = false;

	uint selectFx, hoverFx;
	
	SDL_Texture* pressedTexture;
	SDL_Texture* focusedTexture;

};

#endif // __GUIBUTTON_H__
