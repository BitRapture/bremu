#ifndef _BREMUCONT_H_
#define _BREMUCONT_H_

#include <SDL.h>
#include "../nescontroller.h"
#include "../../defs.h"

// BREMU exclusive
// SDL derivative controller
struct bremucont : public nescontroller
{
public:
	void UpdateController() override
	{
		m_Buttons = 0; // Reset buttons
		const u8* state = SDL_GetKeyboardState(nullptr);
		m_Buttons |= (state[SDL_SCANCODE_Z] << 7); // A
		m_Buttons |= (state[SDL_SCANCODE_X] << 6); // B
		m_Buttons |= (state[SDL_SCANCODE_A] << 5); // Select
		m_Buttons |= ((state[SDL_SCANCODE_S] | state[SDL_SCANCODE_RETURN]) << 4); // Start
		m_Buttons |= (state[SDL_SCANCODE_UP] << 3); // Up
		m_Buttons |= (state[SDL_SCANCODE_DOWN] << 2); // Down
		m_Buttons |= (state[SDL_SCANCODE_LEFT] << 1); // Left
		m_Buttons |= (state[SDL_SCANCODE_RIGHT]); // Right
	}
};

#endif // !_BREMUCONT_H_