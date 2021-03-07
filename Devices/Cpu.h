//=======
// Cpu.h
//=======

#pragma once


//===========
// Namespace
//===========

namespace Devices {


//=====
// Cpu
//=====

class Cpu
{
public:
	// Common
	static inline VOID Abort()
		{
		__asm("break 0,0");
		}
	static VOID DisableInterrupts();
	static VOID DisableWatchDogTimer();
	static VOID EnableInterrupts();
	static VOID EnableWatchDogTimer();
	static const UINT Frequency=160000000;
	static inline UINT GetCycleCount() __attribute__((always_inline))
		{
		UINT ccount;
		__asm __volatile("rsr %0,ccount":"=a" (ccount));
		return ccount;
		}
	static inline VOID Sleep(UINT MicroSeconds)
		{
		UINT ustart=GetCycleCount();
		UINT uend=ustart+MicroSeconds*160;
		if(uend<ustart)
			while(GetCycleCount()>ustart);
		while(GetCycleCount()<uend);
		}

protected:
	// Con-/Destructors
	Cpu() {}
};

}
