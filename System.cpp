//============
// System.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include <esp_sleep.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include "Devices/Storage/FlashDevice.h"
#include "System.h"

using namespace Devices;
using namespace Devices::Storage;
using namespace Physics;


//========
// Common
//========

WakeupReason System::GetWakeupReason()
{
auto cause=esp_sleep_get_wakeup_cause();
switch(cause)
	{
	case ESP_SLEEP_WAKEUP_TIMER:
		return WakeupReason::Timer;
	case ESP_SLEEP_WAKEUP_TOUCHPAD:
		return WakeupReason::Touchpad;
	default:
		break;
	}
return WakeupReason::None;
}

Handle<Memory> System::Memory;

VOID System::Reset()
{
auto hflash=FlashDevice::Current;
if(hflash)
	{
	UINT usize=hflash->GetSize();
	hflash->Erase(0, usize);
	}
esp_restart();
}

VOID System::Restart()
{
DebugPrint("Restart...\n");
esp_restart();
}

VOID System::Sleep(UINT ums)
{
esp_wifi_stop();
esp_deep_sleep(ums*1000);
}

VOID System::SleepTouchable(UINT ums)
{
esp_sleep_enable_touchpad_wakeup();
Sleep(ums);
}

Handle<TimePoint> System::Start=new TimePoint("SystemStart");

const FLOAT System::Voltage=3.3f;
