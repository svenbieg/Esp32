//===========
// Mutex.cpp
//===========

#include "pch.h"


//=======
// Using
//=======

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Mutex.h"


//==================
// Con-/Destructors
//==================

Mutex::Mutex()
{
portMUX_TYPE* pcs=(portMUX_TYPE*)&uMutex;
pcs->count=portMUX_FREE_VAL;
pcs->owner=0;
portENTER_CRITICAL(pcs);
}

Mutex::~Mutex()
{
Release();
}


//========
// Common
//========

VOID Mutex::Release()
{
portMUX_TYPE* pcs=(portMUX_TYPE*)&uMutex;
if(pcs->count!=portMUX_FREE_VAL)
	{
	portEXIT_CRITICAL(pcs);
	pcs->count=portMUX_FREE_VAL;
	pcs->owner=0;
	}
}
