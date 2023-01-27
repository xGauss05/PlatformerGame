#include "GuiSlider.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Log.h"
#include "Textures.h"

GuiSlider::GuiSlider(uint32 id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::CHECKBOX, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;

	selectFx = app->audio->LoadFx("Assets/Audio/Fx/metal.wav");
	hoverFx = app->audio->LoadFx("Assets/Audio/Fx/hover.wav");

	length = 500;
	buttonBox = bounds;
	sliderLine = { bounds.x, (int)(bounds.y + bounds.h * 0.5f), length, 2 };
	value = 0;

}

GuiSlider::~GuiSlider()
{

}

bool GuiSlider::Update(float dt)
{
	if (state != GuiControlState::DISABLED)
	{

		app->input->GetMousePosition(mouseX, mouseY);
		GuiControlState previousState = state;

		// I'm inside the limits of the sliderbox
		if (mouseX >= buttonBox.x && mouseX <= buttonBox.x + buttonBox.w &&
			mouseY >= buttonBox.y && mouseY <= buttonBox.y + buttonBox.h)
		{

			state = GuiControlState::FOCUSED;
			if (previousState != state)
			{
				// sound maybe?
			}

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT)
			{
				state = GuiControlState::PRESSED;
				int updateValue = value + mouseX - mousePrevX;
				SetValue(updateValue);
				NotifyObserver();
			}
		}
		else {
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT &&
				state == GuiControlState::PRESSED)
			{
				int updateValue = value + mouseX - mousePrevX;
				SetValue(updateValue);
				NotifyObserver();
			}
			else
			{
				if (state != GuiControlState::NORMAL) state = GuiControlState::NORMAL;
			}
		}
		mousePrevX = mouseX;
	}

	return false;
}

bool GuiSlider::Draw(Render* render)
{
	if (state != GuiControlState::DISABLED)
	{
		if (texture == NULL)
		{

			render->DrawRectangle(sliderLine, 100, 100, 100, 255, true, false);
			switch (state)
			{
			case GuiControlState::NORMAL:
				render->DrawRectangle(buttonBox, 0, 0, 255, 255, true, false);
				break;
			case GuiControlState::FOCUSED:
				render->DrawRectangle(buttonBox, 0, 0, 20, 255, true, false);
				break;
			case GuiControlState::PRESSED:
				render->DrawRectangle(buttonBox, 0, 255, 0, 255, true, false);
				break;
			}

		}
		else
		{
			/*if (state != GuiControlState::DISABLED)
				render->DrawTexture(texture, bounds.x, bounds.y);*/
		}
	}
	return false;
}

void GuiSlider::SetValue(int value)
{
	this->value = value;
	if (value <= 0) value = 0;
	else if (value >= length) value = length;

	buttonBox.x = bounds.x + value;
}
