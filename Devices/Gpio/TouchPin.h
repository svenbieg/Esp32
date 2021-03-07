//============
// TouchPin.h
//============

#pragma once


//=======
// Using
//=======

#include "BoolClass.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//===========
// Touch-Pin
//===========

class TouchPin: public Object
{
public:
	// Con-/Destructors
	TouchPin(Handle<String> Id, BYTE Pin);
	~TouchPin();

	// Common
	BYTE GetPin()const { return uPin; }
	Event<TouchPin> Released;
	Event<TouchPin> Touched;
	Handle<Bool> Value;

private:
	// Common
	WORD InitializePin();
	VOID InitializeTouchpad();
	VOID OnLoop();
	UINT uAverage;
	UINT uAvgCount;
	static UINT uPinCount;
	BYTE uPin;
};

}}