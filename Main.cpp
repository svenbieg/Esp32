//=================
// Application.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include <time.h>
#include "Core/Application.h"
#include "Devices/Serial/SerialPort.h"
#include "Devices/Clock.h"
#include "System.h"

using namespace Core;
using namespace Devices;
using namespace Devices::Serial;

extern VOID Initialize();

#include "User/Storage/Buffer.h"


//=============
// Entry-Point
//=============

extern "C" VOID app_main()
{
setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", true);
tzset();
System::Memory=new Memory();
Clock::Current=new Clock();
SerialPort::Current=new SerialPort(0, BaudRate::Baud115200);
auto pdesc=esp_ota_get_app_description();
Application::Version=pdesc->version;
Initialize();
auto happ=Application::Current;
System::Start=Clock::Current->Now();
while(1)
	{
	System::Memory->Update();
	happ->Loop(happ);
	vTaskDelay(10);
	}
}
