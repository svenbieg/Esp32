//==================
// AnalogHelper.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include <esp_adc_cal.h>
#include <math.h>
#include "AnalogHelper.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//=========
// Globals
//=========

BOOL bAdcInitialized=false;
esp_adc_cal_characteristics_t cCharacteristics;


//=================
// Analog Channels
//=================

const adc1_channel_t pAnalogChannels[]={ ADC1_CHANNEL_4, ADC1_CHANNEL_5, ADC1_CHANNEL_6, ADC1_CHANNEL_7, ADC1_CHANNEL_0, ADC1_CHANNEL_MAX, ADC1_CHANNEL_MAX, ADC1_CHANNEL_3 };


//========
// Common
//========

FLOAT AnalogRead(BYTE upin)
{
adc1_channel_t ch=pAnalogChannels[upin-32];
if(!bAdcInitialized)
	{
	adc_power_on();
	adc_set_clk_div(1);
	adc1_config_width(ADC_WIDTH_BIT_12);
	esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &cCharacteristics);
	bAdcInitialized=true;
	}
adc1_config_channel_atten(ch, ADC_ATTEN_DB_11);
INT input=adc1_get_raw(ch);
if(input==4095)
	return -1.f;
UINT uraw=adc1_get_raw(ch);
UINT uvoltage=esp_adc_cal_raw_to_voltage(uraw, &cCharacteristics);
return (FLOAT)uvoltage/1000.f;
}

}}
