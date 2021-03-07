//=========
// Cpu.cpp
//=========

#include "pch.h"


//=======
// Using
//=======

extern "C"
{
VOID disableCore0WDT();
VOID enableCore0WDT();
VOID ets_intr_lock();
VOID ets_intr_unlock();
}

#include "Cpu.h"


//===========
// Namespace
//===========

namespace Devices {


//========
// Common
//========

VOID Cpu::DisableInterrupts()
{
ets_intr_lock();
}

VOID Cpu::DisableWatchDogTimer()
{
disableCore0WDT();
}

VOID Cpu::EnableInterrupts()
{
ets_intr_unlock();
}

VOID Cpu::EnableWatchDogTimer()
{
enableCore0WDT();
}

}
