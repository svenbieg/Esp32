//==========
// System.h
//==========

#pragma once


//=======
// Using
//=======

#include "Devices/MemoryClass.h"
#include "Physics/Temperature.h"
#include "Physics/TimePoint.h"
#include "FloatClasses.h"


//===============
// Wakeup-Reason
//===============

enum class WakeupReason
{
None,
Timer,
Touchpad
};


//========
// System
//========

class System
{
public:
	// Common
	static WakeupReason GetWakeupReason();
	static Handle<Devices::Memory> Memory;
	static VOID Reset();
	static VOID Restart();
	static VOID Sleep(UINT Millis);
	static VOID SleepTouchable(UINT Millis);
	static Handle<Physics::TimePoint> Start;
	static const FLOAT Voltage;
};
