//==============
// TouchPin.cpp
//==============

#include "pch.h"


//=======
// Using
//=======

#include <driver/touch_pad.h>
#include "TouchPin.h"

using namespace Runtime;


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//==========
// Channels
//==========

touch_pad_t pTouchChannels[]={ TOUCH_PAD_NUM1, TOUCH_PAD_MAX, TOUCH_PAD_NUM2, TOUCH_PAD_MAX, TOUCH_PAD_NUM0,
	TOUCH_PAD_MAX, TOUCH_PAD_MAX, TOUCH_PAD_MAX, TOUCH_PAD_MAX, TOUCH_PAD_MAX, TOUCH_PAD_MAX, TOUCH_PAD_MAX,
	TOUCH_PAD_NUM5, TOUCH_PAD_NUM4, TOUCH_PAD_NUM6, TOUCH_PAD_NUM3,
	TOUCH_PAD_MAX, TOUCH_PAD_MAX, TOUCH_PAD_MAX, TOUCH_PAD_MAX, TOUCH_PAD_MAX, TOUCH_PAD_MAX, TOUCH_PAD_MAX, TOUCH_PAD_MAX, TOUCH_PAD_MAX, TOUCH_PAD_MAX, TOUCH_PAD_MAX,
	TOUCH_PAD_NUM7, TOUCH_PAD_MAX, TOUCH_PAD_MAX, TOUCH_PAD_MAX, TOUCH_PAD_MAX, TOUCH_PAD_NUM9, TOUCH_PAD_NUM8 };


//==================
// Con-/Destructors
//==================

TouchPin::TouchPin(Handle<String> hid, BYTE upin):
uAverage(0),
uAvgCount(0),
uPin(upin)
{
Value=new Bool(hid);
if(uPin>=ARRAYSIZE(pTouchChannels)||pTouchChannels[uPin]==TOUCH_PAD_MAX)
	{
	DebugPrint("Invalid Touch Pin %u\n", uPin);
	return;
	}
if(uPinCount++==0)
	InitializeTouchpad();
Application::Current->Loop.Add(this, &TouchPin::OnLoop);
}

TouchPin::~TouchPin()
{
if(--uPinCount==0)
	touch_pad_deinit();
}


//================
// Common Private
//================

WORD TouchPin::InitializePin()
{
if(uAvgCount>32)
	return (WORD)uAverage;
touch_pad_t uch=pTouchChannels[uPin];
if(uAvgCount<32)
	{
	WORD uvalue=0;
	touch_pad_read(uch, &uvalue);
	uAverage+=uvalue;
	uAvgCount++;
	}
else if(uAvgCount==32)
	{
	uAverage=uAverage/32-100;
	if(uAverage<200)
		{
		DebugPrint("Touch-Pin %u average too low\n", uPin);
		Application::Current->Loop.Remove(this);
		return 0;
		}
	touch_pad_config(uch, (WORD)uAverage);
	DebugPrint("Touch-Pin %u threshold\n", uPin, uAverage);
	uAvgCount++;
	}
return 0;
}

VOID TouchPin::InitializeTouchpad()
{
if(touch_pad_init()!=ESP_OK)
	{
	DebugPrint("touch_pad_init() failed\n");
	}
touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
for(BYTE u=0; u<TOUCH_PAD_MAX; u++)
	touch_pad_config((touch_pad_t)u, 0);
}

VOID TouchPin::OnLoop()
{
WORD uthreshold=InitializePin();
if(!uthreshold)
	return;
touch_pad_t uch=pTouchChannels[uPin];
WORD uvalue=0;
touch_pad_read(uch, &uvalue);
BOOL b=uvalue<uthreshold;
BOOL bold=Value;
if(b==bold)
	return;
Value->Set(b);
b? Touched(this): Released(this);
}

UINT TouchPin::uPinCount=0;

}}