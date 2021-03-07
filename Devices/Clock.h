//=========
// Clock.h
//=========

#pragma once


//=======
// Using
//=======

#include "Physics/TimePoint.h"


//===========
// Namespace
//===========

namespace Devices {


//=============
// Clock-Flags
//=============

enum class ClockFlags
{
None=0,
Ticking=1
};


//=======
// Clock
//=======

class Clock: public Object
{
private:
	// Using
	using TIMEPOINT=Physics::TIMEPOINT;
	using TimePoint=Physics::TimePoint;

public:
	// Con-/Destructors
	Clock();

	// Common
	static Handle<Clock> Current;
	Event<Handle<Clock>> Day;
	Event<Handle<Clock>> Hour;
	INT GetDayOfMonth()const { return iDayOfMonth; }
	INT GetDayOfYear()const;
	BOOL IsSet()const { return iYear!=0; }
	INT GetHour()const { return iHour; }
	INT GetMinute()const { return iMinute; }
	INT GetYear()const { return iYear; }
	Event<Handle<Clock>> Minute;
	Event<Handle<Clock>> Month;
	TIMEPOINT Now();
	VOID SetTime(Handle<TimePoint> Time);
	VOID StopSync();
	VOID Sync();
	Event<Handle<Clock>> TimeSet;
	BOOL Update(TIMEPOINT* TimePoint);
	Event<Handle<Clock>> Year;

private:
	// Common
	static VOID Callback(VOID* Time);
	VOID InitTime();
	VOID OnLoop();
	INT iDayOfMonth;
	INT iHour;
	INT iMinute;
	INT iMonth;
	INT iYear;
	ClockFlags uFlags;
};

}
