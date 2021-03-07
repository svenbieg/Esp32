//=================
// ClockHelper.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include <esp_ota_ops.h>
#include <esp_sntp.h>
#include "Devices/Clock.h"
#include "Devices/ClockHelper.h"


//===========
// Namespace
//===========

namespace Devices {


//==========
// Callback
//==========

VOID ClockCallback(timeval* pt)
{
if(sntp_get_sync_status()!=SNTP_SYNC_STATUS_COMPLETED)
	return;
esp_ota_mark_app_valid_cancel_rollback();
}


//========
// Common
//========

VOID ClockStartSync()
{
ClockStopSync();
sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
sntp_setoperatingmode(SNTP_OPMODE_POLL);
sntp_setservername(0, "pool.ntp.org");
sntp_set_time_sync_notification_cb(ClockCallback);
sntp_init();
}

VOID ClockStopSync()
{
if(sntp_enabled())
	sntp_stop();
}

}