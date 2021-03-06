#include "common.h"
#include "CommonTypes.h"
#include "gcenv.h"

#include "gc_object.h"

// These are the only interactions with the memory manager inside Autocrat
extern "C" void* allocate_bytes(size_t bytes);

namespace
{
    Object* allocate_object(EEType* type, std::size_t size)
    {
        auto handle = static_cast<gc_object*>(allocate_bytes(sizeof(gc_object) + size));
        auto object = reinterpret_cast<Object*>(handle->object);
        object->set_EEType(type);
        return object;
    }
}

struct RH_GC_GENERATION_INFO
{
    UInt64 sizeBefore;
    UInt64 fragmentationBefore;
    UInt64 sizeAfter;
    UInt64 fragmentationAfter;
};

#if defined(TARGET_X86) && !defined(TARGET_UNIX)
#include "pshpack4.h"
#ifdef _MSC_VER 
#pragma warning(push)
#pragma warning(disable:4121) // alignment of a member was sensitive to packing
#endif
#endif
struct RH_GH_MEMORY_INFO
{
public:
    UInt64 highMemLoadThresholdBytes;
    UInt64 totalAvailableMemoryBytes;
    UInt64 lastRecordedMemLoadBytes;
    UInt64 lastRecordedHeapSizeBytes;
    UInt64 lastRecordedFragmentationBytes;
    UInt64 totalCommittedBytes;
    UInt64 promotedBytes;
    UInt64 pinnedObjectCount;
    UInt64 finalizationPendingCount;
    UInt64 index;
    UInt32 generation;
    UInt32 pauseTimePercent;
    UInt8 isCompaction;
    UInt8 isConcurrent;
    RH_GC_GENERATION_INFO generationInfo0;
    RH_GC_GENERATION_INFO generationInfo1;
    RH_GC_GENERATION_INFO generationInfo2;
    RH_GC_GENERATION_INFO generationInfo3;
    RH_GC_GENERATION_INFO generationInfo4;
    UInt64 pauseDuration0;
    UInt64 pauseDuration1;
};
#if defined(TARGET_X86) && !defined(TARGET_UNIX)
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include "poppack.h"
#endif

COOP_PINVOKE_HELPER(Boolean, RhCancelFullGCNotification, ())
{
    return Boolean_true;
}

COOP_PINVOKE_HELPER(Int64, RhGetAllocatedBytesForCurrentThread, ())
{
    return 0;
}

COOP_PINVOKE_HELPER(Int64, RhGetCurrentObjSize, ())
{
    return 0;
}

COOP_PINVOKE_HELPER(Int64, RhGetGCNow, ())
{
    return 0;
}

COOP_PINVOKE_HELPER(Int64, RhGetGCSegmentSize, ())
{
    return 0;
}

COOP_PINVOKE_HELPER(Int32, RhGetLohCompactionMode, ())
{
    return 0;
}

COOP_PINVOKE_HELPER(Int64, RhGetLastGCDuration, (Int32 generation))
{
    return 0;
}

COOP_PINVOKE_HELPER(Int64, RhGetLastGCStartTime, (Int32 generation))
{
    return 0;
}

COOP_PINVOKE_HELPER(Int32, RhGetMaxGcGeneration, ())
{
    return 0;
}

COOP_PINVOKE_HELPER(void, RhGetMemoryInfo, (RH_GH_MEMORY_INFO* pData, int kind))
{
    ((void)kind);
    *pData = {};
}

COOP_PINVOKE_HELPER(Int32, RhGetGcCollectionCount, (Int32 generation, Boolean getSpecialGCCount))
{
    ((void)generation);
    ((void)getSpecialGCCount);
    return 0;
}

COOP_PINVOKE_HELPER(Int32, RhGetGeneration, (OBJECTREF obj))
{
    ((void)obj);
    return 0;
}

COOP_PINVOKE_HELPER(Int32, RhGetGcLatencyMode, ())
{
    return 0;
}

EXTERN_C REDHAWK_API Int64 __cdecl RhGetTotalAllocatedBytesPrecise()
{
    return 0;
}

EXTERN_C REDHAWK_API void __cdecl RhpCollect(UInt32 uGeneration, UInt32 uMode)
{
    ((void)uGeneration);
    ((void)uMode);
}

EXTERN_C REDHAWK_API Int32 __cdecl RhpEndNoGCRegion()
{
    return 0;
}

EXTERN_C REDHAWK_API Int64 __cdecl RhpGetGcTotalMemory()
{
    return 0;
}

COOP_PINVOKE_HELPER(Boolean, RhIsPromoted, (OBJECTREF obj))
{
    ((void)obj);
    return Boolean_false;
}

COOP_PINVOKE_HELPER(Boolean, RhIsServerGc, ())
{
    return Boolean_false;
}

COOP_PINVOKE_HELPER(Array*, RhpNewArray, (EEType* pArrayEEType, int numElements))
{
    size_t size = (size_t)pArrayEEType->get_BaseSize() + ((size_t)numElements * (size_t)pArrayEEType->get_ComponentSize());
    size = ALIGN_UP(size, sizeof(UIntNative));

    auto array = (Array*)allocate_object(pArrayEEType, size);
    array->InitArrayLength(numElements);
    return array;
}

COOP_PINVOKE_HELPER(Object*, RhpNewFast, (EEType* pEEType))
{
    return allocate_object(pEEType, pEEType->get_BaseSize());
}

COOP_PINVOKE_HELPER(Object*, RhpNewFinalizable, (EEType* pEEType))
{
    return allocate_object(pEEType, pEEType->get_BaseSize());
}

EXTERN_C REDHAWK_API void RhAllocateNewArray(EEType* pArrayEEType, UInt32 numElements, UInt32 flags, Array** pResult)
{
    *pResult = RhpNewArray(pArrayEEType, numElements);
}

COOP_PINVOKE_HELPER(String*, RhNewString, (EEType* pArrayEEType, int numElements))
{
    return (String*)RhpNewArray(pArrayEEType, numElements);
}

EXTERN_C REDHAWK_API Int32 __cdecl RhpStartNoGCRegion(Int64 totalSize, Boolean hasLohSize, Int64 lohSize, Boolean disallowFullBlockingGC)
{
    ((void)totalSize);
    ((void)hasLohSize);
    ((void)lohSize);
    ((void)disallowFullBlockingGC);
    return 0;
}

COOP_PINVOKE_HELPER(Boolean, RhRegisterGcCallout, (GcRestrictedCalloutKind eKind, void * pCallout))
{
    ((void)eKind);
    ((void)pCallout);
    return Boolean_true;
}

COOP_PINVOKE_HELPER(Boolean, RhReRegisterForFinalize, (OBJECTREF refObj))
{
    ((void)refObj);
    return Boolean_true;
}

COOP_PINVOKE_HELPER(Boolean, RhRegisterForFullGCNotification, (Int32 maxGenerationThreshold, Int32 largeObjectHeapThreshold))
{
    ((void)maxGenerationThreshold);
    ((void)largeObjectHeapThreshold);
    return Boolean_true;
}

COOP_PINVOKE_HELPER(Int32, RhSetGcLatencyMode, (Int32 newLatencyMode))
{
    ((void)newLatencyMode);
    return 0;
}

COOP_PINVOKE_HELPER(void, RhSetLohCompactionMode, (Int32 newLohCompactionMode))
{
    ((void)newLohCompactionMode);
}

COOP_PINVOKE_HELPER(void, RhSuppressFinalize, (OBJECTREF refObj))
{
    ((void)refObj);
}

COOP_PINVOKE_HELPER(void, RhUnregisterGcCallout, (GcRestrictedCalloutKind eKind, void* pCallout))
{
    ((void)eKind);
    ((void)pCallout);
}

COOP_PINVOKE_HELPER(Int32, RhWaitForFullGCApproach, (Int32 millisecondsTimeout))
{
    ((void)millisecondsTimeout);
    return 0;
}

COOP_PINVOKE_HELPER(Int32, RhWaitForFullGCComplete, (Int32 millisecondsTimeout))
{
    ((void)millisecondsTimeout);
    return 0;
}
