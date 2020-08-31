#include <atomic>
#include "common.h"
#include "gcenv.h"
#include "objecthandle.h"

#include "gc_object.h"

namespace
{
    gc_handle* get_handle_from_object(void* object)
    {
        if (object == nullptr)
        {
            return nullptr;
        }
        else
        {
            return &static_cast<gc_object*>(object)[-1].handle;
        }
    }

    void* get_object_from_handle(void* handle)
    {
        if (handle == nullptr)
        {
            return nullptr;
        }
        else
        {
            return &static_cast<gc_object*>(handle)->object;
        }
    }
}

COOP_PINVOKE_HELPER(OBJECTHANDLE, RhpHandleAlloc, (Object* pObject, int type))
{
    ((void)type);
    gc_handle* handle = get_handle_from_object(pObject);
    handle->object = pObject;
    return reinterpret_cast<OBJECTHANDLE>(handle);
}

COOP_PINVOKE_HELPER(OBJECTHANDLE, RhpHandleAllocDependent, (Object* pPrimary, Object* pSecondary))
{
    ((void)pSecondary);
    return RhpHandleAlloc(pPrimary, -1);
}

COOP_PINVOKE_HELPER(void, RhHandleFree, (OBJECTHANDLE handle))
{
    ((void)handle);
}

COOP_PINVOKE_HELPER(Object*, RhHandleGetDependent, (OBJECTHANDLE handle, Object** ppSecondary))
{
    ppSecondary = nullptr;
    return static_cast<Object*>(get_object_from_handle(handle));
}

COOP_PINVOKE_HELPER(void, RhHandleSet, (OBJECTHANDLE handle, Object* pObject))
{
    if (handle)
    {
        reinterpret_cast<gc_handle*>(handle)->object = pObject;
        std::atomic_thread_fence(std::memory_order_seq_cst);
    }
}
