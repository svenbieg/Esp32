//=================
// MemoryClass.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include <esp_heap_caps.h>
#include "MemoryClass.h"


//===========
// Namespace
//===========

namespace Devices {


//==================
// Con-/Destructors
//==================

Memory::Memory()
{
multi_heap_info_t info;
heap_caps_get_info(&info, MALLOC_CAP_INTERNAL);
Available=new Int32("MemoryAvailable", info.total_free_bytes);
Total=new Int32("MemoryTotal", info.total_free_bytes+info.total_allocated_bytes);
}


//========
// Common
//========

VOID Memory::Update()
{
multi_heap_info_t info;
heap_caps_get_info(&info, MALLOC_CAP_INTERNAL);
Available=info.total_free_bytes;
}

}
