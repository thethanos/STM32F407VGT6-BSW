#ifndef DET_H
#define DET_H

#include "Std_Types.h"

#define DET_MODULE_ID   ((uint16)15)

Std_ReturnType Det_ReportError(uint16 ModuleId, uint8 InstacneId, uint8 ApiId, uint8 ErrorId);

#endif //DET_H