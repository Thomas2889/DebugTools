#pragma once

#include <cstddef>


void setup_heap_monitor();
void check_heap_allocations();
void *operator new(std::size_t count, const char *file, int line);
void *operator new[](std::size_t count, const char *file, int line);
void operator delete(void *p, const char *file, int line);
void operator delete[](void *p, const char *file, int line);

#ifdef DEBUG
#define TRACE_SETUP setup_heap_monitor()
#define TRACE_CHECK check_heap_allocations()
#define TRACE_NEW new (__FILE__ + SOURCE_PATH_LEN, __LINE__)
#define TRACE_DELETE(x) operator delete(x, __FILE__ + SOURCE_PATH_LEN, __LINE__)
#define TRACE_DELETE_ARRAY(x) operator delete[](x, __FILE__ + SOURCE_PATH_LEN, __LINE__)
#else
#define TRACE_SETUP
#define TRACE_CHECK
#define TRACE_NEW new
#define TRACE_DELETE(x) delete x
#define TRACE_DELETE_ARRAY(x) delete[] x
#endif
