#include "common.h"
#include "gcenv.h"

void RhEnableFinalization()
{
}

bool RhInitializeFinalization()
{
    return true;
}

EXTERN_C REDHAWK_API void __cdecl RhInitializeFinalizerThread()
{
}

COOP_PINVOKE_HELPER(OBJECTREF, RhpGetNextFinalizableObject, ())
{
    return nullptr;
}

EXTERN_C REDHAWK_API void __cdecl RhpSignalFinalizationComplete()
{
}

EXTERN_C REDHAWK_API UInt32_BOOL __cdecl RhpWaitForFinalizerRequest()
{
    return TRUE;
}
