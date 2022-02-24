#ifndef _NESCONTROLLER_H_
#define _NESCONTROLLER_H_

#include "../defs.h"

// The base class interface for user interaction
struct nescontroller
{
public: // Variables
	// All buttons from the controller
	u8 m_Buttons{ 0 }; 
public: // Virtual methods
	virtual void UpdateController() = 0;
};

#endif // !_NESCONTROLLER_H_
