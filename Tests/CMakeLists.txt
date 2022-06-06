cmake_minimum_required(VERSION 3.14)

add_executable(DebugToolsTests
	"Tests/HeapMonitor/HeapMonitorTest.cpp"
)

setup_exe_include(DebugToolsTests)

add_libraries(DebugToolsTests)

target_link_libraries(DebugToolsTests PRIVATE gtest_main)
target_link_libraries(DebugToolsTests PUBLIC DebugTools)
target_include_directories(DebugToolsTests PUBLIC "${CMAKE_SOURCE_DIR}/DebugTools")

copy_binaries(DebugToolsTests)

add_test(NAME my_test COMMAND DebugToolsTests)
