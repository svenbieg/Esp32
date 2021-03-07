//===========
// Clock.cpp
//===========

#include "pch.h"


//=======
// Using
//=======

#include <esp_ota_ops.h>
#include <esp_sntp.h>
#include <time.h>
#include "Runtime/Application.h"
#include "Clock.h"

using namespace Physics;
using namespace Runtime;


//===========
// Namespace
//===========

namespace Devices {


//==================
// Helper-Functions
//==================

VOID TimePointFromTimeStruct(TIMEPOINT* Time, tm const* Struct)
{
Time->Second=Struct->tm_sec;
Time->Minute=Struct->tm_min;
Time->Hour=Struct->tm_hour;
Time->DayOfWeek=Struct->tm_wday;
Time->DayOfMonth=Struct->tm_mday;
Time->Month=Struct->tm_mon;
Time->Year=Struct->tm_year;
}


//============
// Year-Table
//============

BYTE DaysPerMonth[4][12]={
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }};


//==================
// Con-/Destructors
//==================

Clock::Clock():
iDayOfMonth(0),
iHour(0),
iMinute(0),
iMonth(0),
iYear(0),
uFlags(ClockFlags::None)
{
setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", true);
tzset();
}


//========
// Common
//========

Handle<Clock> Clock::Current=new Clock();

INT Clock::GetDayOfYear()const
{
if(iYear==0)
	return 0;
UINT uyear=iYear;
WORD uyear4=uyear%4;
INT iday=0;
for(INT i=0; i<iMonth-1; i++)
	iday+=DaysPerMonth[uyear4][i];
iday+=iDayOfMonth;
return iday;
}

TIMEPOINT Clock::Now()
{
TIMEPOINT tp;
ZeroMemory(&tp, sizeof(TIMEPOINT));
tm t;
ZeroMemory(&t, sizeof(tm));
if(GetFlag(uFlags, ClockFlags::Ticking))
	{
	time_t now;
	time(&now);
	localtime_r(&now, &t);
	TimePointFromTimeStruct(&tp, &t);
	return tp;
	}
UINT utime=GetTickCount();
tp.DayOfWeek=(BYTE)(utime>>24);
tp.Hour=(BYTE)(utime>>16);
tp.Minute=(BYTE)(utime>>8);
tp.Second=(BYTE)utime;
return tp;
}

VOID Clock::SetTime(Handle<TimePoint> htp)
{
if(!htp)
	return;
tm* pt=(tm*)&htp->Get();
if(pt->tm_year==0)
	return;
time_t t=mktime(pt);
timeval tv;
ZeroMemory(&tv, sizeof(timeval));
tv.tv_sec=t;
settimeofday(&tv, nullptr);
InitTime();
}

VOID Clock::StopSync()
{
if(sntp_enabled())
	sntp_stop();
}

VOID Clock::Sync()
{
if(sntp_enabled())
	sntp_stop();
sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
sntp_setoperatingmode(SNTP_OPMODE_POLL);
sntp_setservername(0, "pool.ntp.org");
sntp_set_time_sync_notification_cb((sntp_sync_time_cb_t)&Callback);
sntp_init();
}

BOOL Clock::Update(TIMEPOINT* ptp)
{
if(!GetFlag(uFlags, ClockFlags::Ticking))
	return false;
UINT uticksnow=GetTickCount();
UINT uticks=((UINT)ptp->DayOfWeek<<24)|((UINT)ptp->Hour<<16)|((UINT)ptp->Minute<<8)|ptp->Second;
time_t t;
time(&t);
t-=(uticksnow-uticks)/1000;
tm ts;
localtime_r(&t, &ts);
TimePointFromTimeStruct(ptp, &ts);
return true;
}


//================
// Common Private
//================

VOID Clock::Callback(VOID* pt)
{
if(sntp_get_sync_status()!=SNTP_SYNC_STATUS_COMPLETED)
	return;
Current->InitTime();
}

VOID Clock::InitTime()
{
if(esp_ota_mark_app_valid_cancel_rollback()!=ESP_OK)
	{
	DebugPrint("esp_ota_mark_app_valid_cancel_rollback() failed\r\n");
	}
time_t now;
time(&now);
tm t;
localtime_r(&now, &t);
iDayOfMonth=t.tm_mday+1;
iHour=t.tm_hour;
iMinute=t.tm_min;
iMonth=t.tm_mon+1;
iYear=t.tm_year+1900;
if(!GetFlag(uFlags, ClockFlags::Ticking))
	{
	Application::Current->Loop.Add(this, &Clock::OnLoop);
	SetFlag(uFlags, ClockFlags::Ticking);
	}
TimeSet(this);
}

VOID Clock::OnLoop()
{
time_t now;
time(&now);
tm t;
localtime_r(&now, &t);
INT ioldyear=iYear;
INT ioldmonth=iMonth;
INT ioldday=iDayOfMonth;
INT ioldhour=iHour;
INT ioldmin=iMinute;
iYear=t.tm_year+1900;
iMonth=t.tm_mon+1;
iDayOfMonth=t.tm_mday+1;
iHour=t.tm_hour;
iMinute=t.tm_min;
if(ioldyear!=iYear)
	Year(this);
if(ioldmonth!=iMonth)
	Month(this);
if(ioldday!=iDayOfMonth)
	Day(this);
if(ioldhour!=iHour)
	Hour(this);
if(ioldmin!=iMinute)
	Minute(this);
}

}
