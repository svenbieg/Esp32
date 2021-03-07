//===========
// RgbLeds.h
//===========

#pragma once


//=======
// Using
//=======

#include "Graphics/Color.h"
#include "User/Storage/Buffer.h"


//===========
// Namespace
//===========

namespace Actors {


//==========
// Rgb-Leds
//==========

class RgbLeds: public Object
{
public:
	// Using
	using COLOR=Graphics::COLOR;

	// Con-/Destructors
	RgbLeds(BYTE Pin, UINT LedCount, BYTE RmtChannel=0);
	~RgbLeds();

	// Common
	VOID Flush();
	BYTE GetPin()const { return uPin; }
	VOID SetColor(UINT Led, COLOR Color, FLOAT Brightness=1.f);

private:
	// Common
	BOOL InitRmt();
	User::Storage::Buffer cColorBuffer;
	User::Storage::Buffer cRmtBuffer;
	BOOL bChanged;
	BYTE uChannel;
	UINT uLedCount;
	BYTE uPin;
};

}