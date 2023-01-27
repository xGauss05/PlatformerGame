#include "GuiCheckbox.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Log.h"
#include "Textures.h"

GuiCheckbox::GuiCheckbox(uint32 id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::CHECKBOX, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;

	selectFx = app->audio->LoadFx("Assets/Audio/Fx/metal.wav");
	hoverFx = app->audio->LoadFx("Assets/Audio/Fx/hover.wav");


}

GuiCheckbox::~GuiCheckbox()
{

}

bool GuiCheckbox::Update(float dt)
{
	if (state != GuiControlState::DISABLED)
	{

		app->input->GetMousePosition(mouseX, mouseY);

		// I'm inside the limits of the checkbox
		if (mouseX >= bounds.x && mouseX <= bounds.x + bounds.w &&
			mouseY >= bounds.y && mouseY <= bounds.y + bounds.h) {

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP)
			{
				NotifyObserver();
				if (state == GuiControlState::SELECTED) state = GuiControlState::NORMAL;
				else if (state == GuiControlState::NORMAL) state = GuiControlState::SELECTED;

				app->audio->PlayFx(selectFx);
			}
		}
	}

	return false;
}

bool GuiCheckbox::Draw(Render* render)
{
	if (texture == NULL)
	{
		switch (state)
		{
		case GuiControlState::NORMAL:
			render->DrawRectangle(bounds, 170, 0, 0, 255, true, false);
			break;
		case GuiControlState::SELECTED:
			render->DrawRectangle(bounds, 0, 255, 0, 255, true, false);
			break;
		}
	}
	else
	{
		if (state != GuiControlState::DISABLED)
			render->DrawTexture(texture, bounds.x, bounds.y);
	}

	return false;
}
