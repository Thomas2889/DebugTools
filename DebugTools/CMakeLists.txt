cmake_minimum_required(VERSION 3.14)

add_library(DebugTools
	 "DebugTools/HeapMonitor/HeapMonitor.cpp" "DebugTools/HeapMonitor/HeapMonitor.h"
)

setup_lib_include(DebugTools)

add_libraries(DebugTools)

copy_headers(DebugTools)
