//=============
// RgbLeds.cpp
//=============

#include "pch.h"


//=======
// Using
//=======

#include <driver/rmt.h>
#include "RgbLeds.h"

using namespace Graphics;


//===========
// Namespace
//===========

namespace Actors {


//==================
// Con-/Destructors
//==================

RgbLeds::RgbLeds(BYTE upin, UINT ucount, BYTE uchannel):
bChanged(true),
uChannel(uchannel),
uLedCount(0),
uPin(upin)
{
if(!InitRmt())
	return;
cColorBuffer.SetSize(ucount*3);
cColorBuffer.Zero();
cRmtBuffer.SetSize(ucount*24*sizeof(rmt_item32_t));
uLedCount=ucount;
Flush();
}

RgbLeds::~RgbLeds()
{
cColorBuffer.Zero();
Flush();
}


//========
// Common
//========

VOID RgbLeds::Flush()
{
if(uLedCount==0||!bChanged)
	return;
rmt_channel_t uchannel=(rmt_channel_t)uChannel;
rmt_wait_tx_done(uchannel, portMAX_DELAY);
BYTE* pcolors=cColorBuffer.Begin();
rmt_item32_t* prmt=(rmt_item32_t*)cRmtBuffer.Begin();
UINT urmt=0;
for(UINT u=0; u<uLedCount*3; u++)
	{
	for(BYTE umask=0x80; umask>0; umask>>=1)
		{
		prmt[urmt].level0=1;
		prmt[urmt].level1=0;
		if(pcolors[u]&umask)
			{
			prmt[urmt].duration0=9;
			prmt[urmt].duration1=3;
			}
		else
			{
			prmt[urmt].duration0=3;
			prmt[urmt].duration1=9;
			}
		urmt++;
		}
	}
rmt_write_items(uchannel, prmt, urmt, false);
bChanged=false;
}

VOID RgbLeds::SetColor(UINT uled, COLOR c, FLOAT fbrightness)
{
if(uled>=uLedCount)
	return;
c.SetBrightness(fbrightness);
BYTE r=c.GetRed();
BYTE g=c.GetGreen();
BYTE b=c.GetBlue();
BYTE* pbuf=cColorBuffer.Begin();
pbuf+=uled*3;
if(pbuf[0]!=r)
	{
	pbuf[0]=r;
	bChanged=true;
	}
if(pbuf[1]!=g)
	{
	pbuf[1]=g;
	bChanged=true;
	}
if(pbuf[2]!=b)
	{
	pbuf[2]=b;
	bChanged=true;
	}
}


//================
// Common Private
//================

BOOL RgbLeds::InitRmt()
{
rmt_config_t cfg;
cfg.rmt_mode=RMT_MODE_TX;
cfg.channel=(rmt_channel_t)uChannel;
cfg.gpio_num=(gpio_num_t)uPin;
cfg.clk_div=8;
cfg.mem_block_num=1;
cfg.tx_config.carrier_freq_hz=100;
cfg.tx_config.carrier_level=RMT_CARRIER_LEVEL_LOW;
cfg.tx_config.idle_level=RMT_IDLE_LEVEL_LOW;
cfg.tx_config.carrier_duty_percent=50;
cfg.tx_config.carrier_en=false;
cfg.tx_config.loop_en=false;
cfg.tx_config.idle_output_en=true;
if(rmt_config(&cfg)!=ESP_OK)
	{
	DebugPrint("rmt_config() failed\n");
	return false;
	}
if(rmt_driver_install(cfg.channel, 0, 0)!=ESP_OK)
	{
	DebugPrint("rmt_driver_install() failed\n");
	return false;
	}
return true;
}

}