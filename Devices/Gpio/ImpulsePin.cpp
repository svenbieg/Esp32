//================
// ImpulsePin.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include <driver/pcnt.h>
#include <esp_attr.h>
#include "ImpulsePin.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//==================
// Con-/Destructors
//==================

ImpulsePin::ImpulsePin(BYTE upin, BYTE uunit):
uCount(0),
uUnit(uunit)
{
pcnt_config_t config;
ZeroMemory(&config, sizeof(pcnt_config_t));
config.unit=(pcnt_unit_t)uunit;
config.ctrl_gpio_num=PCNT_PIN_NOT_USED;
config.pulse_gpio_num=upin;
config.pos_mode=PCNT_COUNT_INC;
ERR_T status=pcnt_unit_config(&config);
if(status!=ESP_OK)
	{
	DebugPrint("pcnt_unit_config() failed %i\n", (INT)status);
	return;
	}
pcnt_counter_clear(config.unit);
hTimer=new Timer();
hTimer->Triggered.Add(this, &ImpulsePin::OnTimerTriggered);
hTimer->StartPeriodic(1000000);
}


//================
// Common Private
//================

VOID ImpulsePin::OnTimerTriggered(Handle<Timer> htimer)
{
pcnt_unit_t unit=(pcnt_unit_t)uUnit;
SHORT icount=0;
if(pcnt_get_counter_value(unit, &icount)!=ESP_OK)
	return;
if(icount<0)
	icount=0;
uCount=icount;
pcnt_counter_clear(unit);
}

}}