#ifndef GC_OBJECT_H
#define GC_OBJECT_H

#include <cstddef>

constexpr std::size_t gc_header_size = 8u;

struct gc_handle
{
    void* object;
    void* extra;
};

struct gc_object
{
    gc_handle handle;
    char header[gc_header_size];
    char object[0];
};

#endif
