#include <gtest/gtest.h>

#include <DebugTools/HeapMonitor/HeapMonitor.h>

#include <fstream>
#include <regex>


TEST(HeapMonitor, DoAllocations) {
	TRACE_SETUP;

	int *num = TRACE_NEW int;
	TRACE_DELETE(num); // normal new


	num = TRACE_NEW int[5];
	TRACE_DELETE_ARRAY(num); // normal new[]

	num = TRACE_NEW int;
	((char *)num - 1)[0] = 0x00;

	char *ptr = TRACE_NEW char;
	(ptr - 1)[0] = 0x00; // underrun and overrun
	(ptr + 1)[0] = 0x00;
	// leave unfreed

	TRACE_CHECK;

	std::ifstream f("heap_monitor.log");
	std::stringstream buffer;
	buffer << f.rdbuf();
	std::string log_contents = buffer.str();

	std::regex addr_regex("0x[0123456789ABCDEF]{16}");
	
	std::regex newline_regex("\\n");
	log_contents = std::regex_replace(log_contents, addr_regex, "0xffffffffffffffff");
	log_contents = std::regex_replace(log_contents, newline_regex, "");

	std::string expected_contents = "[INFO] Allocation 0xffffffffffffffff of 4 bytes at DebugTools\\Tests\\Tests\\HeapMonitor\\HeapMonitorTest.cpp:12[INFO] Deallocation 0xffffffffffffffff of 4 bytes at DebugTools\\Tests\\Tests\\HeapMonitor\\HeapMonitorTest.cpp:13[INFO] Allocation 0xffffffffffffffff of 20 bytes at DebugTools\\Tests\\Tests\\HeapMonitor\\HeapMonitorTest.cpp:16[INFO] Deallocation 0xffffffffffffffff of 20 bytes at DebugTools\\Tests\\Tests\\HeapMonitor\\HeapMonitorTest.cpp:17[INFO] Allocation 0xffffffffffffffff of 4 bytes at DebugTools\\Tests\\Tests\\HeapMonitor\\HeapMonitorTest.cpp:19[INFO] Allocation 0xffffffffffffffff of 1 bytes at DebugTools\\Tests\\Tests\\HeapMonitor\\HeapMonitorTest.cpp:22-------------------- Checking Heap --------------------[INFO] Unfreed memory 0xffffffffffffffff of 1 bytes allocated at DebugTools\\Tests\\Tests\\HeapMonitor\\HeapMonitorTest.cpp:22    [ERROR] pad check failed: underrun detected for allocation at DebugTools\\Tests\\Tests\\HeapMonitor\\HeapMonitorTest.cpp:22    [ERROR] pad check failed: overrun detected for allocation at DebugTools\\Tests\\Tests\\HeapMonitor\\HeapMonitorTest.cpp:22[INFO] Unfreed memory 0xffffffffffffffff of 4 bytes allocated at DebugTools\\Tests\\Tests\\HeapMonitor\\HeapMonitorTest.cpp:19    [ERROR] pad check failed: underrun detected for allocation at DebugTools\\Tests\\Tests\\HeapMonitor\\HeapMonitorTest.cpp:19[INFO] 2 leaks totalling 5 bytes";

	ASSERT_EQ(log_contents, expected_contents);
}
