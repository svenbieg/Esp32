//=================
// TaskClasses.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "TaskClasses.h"


//===========
// Namespace
//===========

namespace Details {


//========================
// Task-Base-Class Common
//========================

VOID TaskBase::Wait()
{
while(GetState()!=TaskState::Done)
	vTaskDelay(1);
}


//==================
// Common Protected
//==================

VOID TaskBase::CreateTask(TaskProc pproc, VOID* pparam, UINT ustack, UINT uprio, UINT ucpu)
{
xTaskCreatePinnedToCore(pproc, "Task", ustack, pparam, uprio, nullptr, ucpu);
}

VOID TaskBase::ExitTask()
{
vTaskDelete(NULL);
}

}
