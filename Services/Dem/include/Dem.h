#ifndef DEM_H
#define DEM_H

#include "Std_Types.h"

#define DEM_MODULE_ID               ((uint16)54)
#define DEM_SETEVENTSTATUS_SID      ((uint8)0x04)

typedef uint8  Dem_EventStatusType;
#define DEM_EVENT_STATUS_PASSED ((Dem_EventIdType)0)
#define DEM_EVENT_STATUS_FAILED ((Dem_EventIdType)1)


typedef uint16 Dem_EventIdType;

Std_ReturnType Dem_SetEventStatus(Dem_EventIdType, Dem_EventStatusType);

#endif // DEM_H