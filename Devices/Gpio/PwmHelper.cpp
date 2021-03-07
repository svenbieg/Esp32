//===============
// PwmHelper.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include <driver/ledc.h>
#include "Devices/Gpio/GpioHelper.h"
#include "PwmHelper.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//=========
// Globals
//=========

BYTE pLedcChannelPin[LEDC_CHANNEL_MAX]={ 0, 0, 0, 0, 0, 0, 0, 0 };
BYTE pLedcChannelTimer[LEDC_CHANNEL_MAX]={ 0, 0, 0, 0, 0, 0, 0, 0 };
BYTE pLedcPins[]={ 5, 17, 18, 19, 21, 22, 23, 25, 26 };
UINT pLedcTimerPeriod[LEDC_TIMER_MAX]={ 0, 0, 0, 0 };
BYTE uLedcChannelCount=0;
BYTE uLedcTimerCount=0;

BYTE LedcGetChannel(BYTE upin)
{
for(BYTE u=0; u<uLedcChannelCount; u++)
	{
	if(pLedcChannelPin[u]==upin)
		return u;
	}
return LEDC_CHANNEL_MAX;
}


//========
// Common
//========

VOID PwmInit(BYTE upin, UINT uperiod)
{
BOOL bvalid=false;
for(BYTE u=0; u<ARRAYSIZE(pLedcPins); u++)
	{
	if(pLedcPins[u]==upin)
		{
		bvalid=true;
		break;
		}
	}
if(!bvalid)
	{
	DebugPrint("PwmInit - Invalid Pin %u\n", upin);
	return;
	}
if(uLedcChannelCount==LEDC_CHANNEL_MAX)
	{
	DebugPrint("PwmInit - Max channels\n");
	return;
	}
BYTE utimer=LEDC_TIMER_MAX;
for(BYTE u=0; u<uLedcTimerCount; u++)
	{
	if(pLedcTimerPeriod[u]==uperiod)
		{
		utimer=u;
		break;
		}
	}
if(utimer==LEDC_TIMER_MAX)
	{
	if(uLedcTimerCount==LEDC_TIMER_MAX)
		{
		DebugPrint("PwmInit - Max timers\n");
		return;
		}
	utimer=uLedcTimerCount;
	ledc_timer_config_t tc;
	ZeroMemory(&tc, sizeof(ledc_timer_config_t));
	tc.duty_resolution=LEDC_TIMER_10_BIT;
	tc.clk_cfg=LEDC_AUTO_CLK;
	tc.freq_hz=1000000/uperiod;
	tc.speed_mode=LEDC_HIGH_SPEED_MODE;
	tc.timer_num=(ledc_timer_t)utimer;
	if(ledc_timer_config(&tc)!=ESP_OK)
		{
		DebugPrint("ledc_timer_config() failed\n");
		return;
		}
	pLedcTimerPeriod[utimer]=uperiod;
	uLedcTimerCount++;
	}
BYTE uchannel=uLedcChannelCount;
ledc_channel_config_t cc;
ZeroMemory(&cc, sizeof(ledc_channel_config_t));
cc.channel=(ledc_channel_t)uchannel;
cc.gpio_num=upin;
cc.timer_sel=(ledc_timer_t)utimer;
cc.speed_mode=LEDC_HIGH_SPEED_MODE;
if(ledc_channel_config(&cc)!=ESP_OK)
	{
	DebugPrint("ledc_channel_config() failed\n");
	return;
	}
pLedcChannelPin[uchannel]=upin;
pLedcChannelTimer[uchannel]=utimer;
uLedcChannelCount++;
}

VOID PwmSetDuty(BYTE upin, UINT udutyus)
{
BYTE uchannel=LedcGetChannel(upin);
if(uchannel==LEDC_CHANNEL_MAX)
	return;
BYTE utimer=pLedcChannelTimer[uchannel];
UINT uperiod=pLedcTimerPeriod[utimer];
UINT uduty=udutyus*1023/uperiod;
/*if(uduty==0||uduty==1023)
	{
	ledc_stop(LEDC_LOW_SPEED_MODE, (ledc_channel_t)uchannel, 0);
	SetPinMode(upin, PinMode::Output);
	DigitalWrite(upin, uduty==1023);
	DebugPrint("ledc_stop()\r\n");
	return;
	}*/
ledc_set_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)uchannel, uduty);
ledc_update_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)uchannel);
DebugPrint("ledc_set_duty - Pin:%u Timer:%u Channel:%u Duty:%u\n", upin, utimer, uchannel, uduty);
}

}}
