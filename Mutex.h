//=========
// Mutex.h
//=========

#pragma once


//=======
// Mutex
//=======

class Mutex
{
public:
	// Con-/Destructors
	Mutex();
	~Mutex();

	// Common
	VOID Release();

private:
	// Common
	UINT64 uMutex;
};
