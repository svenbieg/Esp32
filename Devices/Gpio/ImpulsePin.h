//==============
// ImpulsePin.h
//==============

#pragma once


//=======
// Using
//=======

#include "Devices/Timer.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//=============
// Impulse-Pin
//=============

class ImpulsePin: public Object
{
public:
	// Con-/Destructors
	ImpulsePin(BYTE Pin, BYTE PcntUnit);

	// Common
	UINT GetImpulseCount()const { return uCount; }

private:
	// Common
	VOID OnTimerTriggered(Handle<Timer> Timer);
	Handle<Timer> hTimer;
	UINT uCount;
	BYTE uUnit;
};

}}