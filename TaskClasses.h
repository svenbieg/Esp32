//===============
// TaskClasses.h
//===============

#pragma once


//=======
// Using
//=======

#include "Function.h"
#include "Procedure.h"
#include "TaskHelper.h"


//============
// Task-State
//============

enum class TaskState
{
Initializing,
Running,
Done
};


//=================
// Task Base-Class
//=================

namespace Details
{
class TaskBase: public Object
	{
	public:
		// Common
		TaskState GetState()const { return uState; }
		VOID Wait();

	protected:
		// Con-/Destructors
		TaskBase(): hThis(this), uState(TaskState::Initializing) {}

		// Task-Proc
		typedef VOID (*TaskProc)(VOID* Param);

		// Common
		VOID CreateTask(TaskProc Proc, VOID* Param, UINT Stack, UINT Priority, UINT Cpu);
		VOID ExitTask();
		Handle<TaskBase> hThis;
		TaskState uState;
	};
}


//======
// Task
//======

class Task: public Details::TaskBase
{
public:
	// Con-/Destructors
	Task(Procedure Procedure, UINT Stack=2048, UINT Priority=5, UINT Cpu=0):
		cProc(Procedure)
		{
		CreateTask(DoRun, this, Stack, Priority, Cpu);
		}

private:
	// Common
	static VOID DoRun(VOID* Param)
		{
		auto ptask=(Task*)Param;
		ptask->uState=TaskState::Running;
		ptask->cProc.Run();
		ptask->uState=TaskState::Done;
		ptask->hThis=nullptr;
		ptask->ExitTask();
		}
	Procedure cProc;
};

using AsyncAction=Handle<Task>;


//===========
// Operation
//===========

template <class _tRet>
class Operation: public Details::TaskBase
{
public:
	// Con-/Destructors
	Operation(Function<_tRet> Function, UINT Stack=2048, UINT Priority=5, UINT Cpu=0):
		cFunc(Function)
		{
		CreateTask(DoRun, this, Stack, Priority, Cpu);
		}

	// Common
	_tRet Get()
		{
		Wait();
		return tRet;
		}

private:
	// Common
	static VOID DoRun(VOID* Param)
		{
		auto pop=(Operation<_tRet>*)Param;
		pop->uState=TaskState::Running;
		pop->tRet=pop->cFunc();
		pop->uState=TaskState::Done;
		pop->hThis=nullptr;
		pop->ExitTask();
		}
	Function<_tRet> cFunc;
	_tRet tRet;
};

template <class _tRet>
using AsyncOperation=Handle<Operation<_tRet>>;
