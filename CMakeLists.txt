cmake_minimum_required(VERSION 3.14)


project("DebugTools" VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "/wd4003")


if(NOT EXISTS "${CMAKE_BINARY_DIR}/Headers")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Headers")
endif()

if(NOT EXISTS "wd")
file(MAKE_DIRECTORY "wd")
endif()


#add_definitions(OS ${CMAKE_OS_NAME})
if(${CMAKE_OS_NAME} STREQUAL "Windows")
	add_compile_definitions(WIN64)
elseif(${CMAKE_OS_NAME} STREQUAL "Linux")
	add_compile_definitions(__unix__)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	add_compile_definitions(DEBUG)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
	add_compile_definitions(RELEASE)
endif()

string(LENGTH "${CMAKE_SOURCE_DIR}" SOURCE_PATH_LEN)
math(EXPR SOURCE_PATH_LEN "${SOURCE_PATH_LEN} - 10")
add_compile_definitions("SOURCE_PATH_LEN=${SOURCE_PATH_LEN}")


#useful functions for subdirs
macro(setup_lib_include LIB)
	target_include_directories(${LIB} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
	target_include_directories(${LIB} PRIVATE "${CMAKE_SOURCE_DIR}/in/Include")
endmacro()
macro(setup_exe_include EXE)
	target_include_directories(${EXE} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/${EXE}")
	target_include_directories(${EXE} PRIVATE "${CMAKE_SOURCE_DIR}/in/Include")
endmacro()

macro(add_libraries LIB)
	if(${CMAKE_OS_NAME} STREQUAL "Windows")
		foreach(LIBRARY ${ARGN})
			target_link_libraries(${LIB} PUBLIC "${CMAKE_SOURCE_DIR}/in/Lib/${CMAKE_OS_NAME}/${CMAKE_PLATFORM_NAME}-${CMAKE_BUILD_TYPE}/${LIBRARY}.lib")
		endforeach()
	elseif(${CMAKE_OS_NAME} STREQUAL "Linux")
		foreach(LIBRARY ${ARGN})
			target_link_libraries(${LIB} PUBLIC "${CMAKE_SOURCE_DIR}/in/Lib/${CMAKE_OS_NAME}/${CMAKE_PLATFORM_NAME}-${CMAKE_BUILD_TYPE}/${LIBRARY}.lib")
		endforeach()
	endif()
endmacro()

macro(add_deps LIB)
	foreach(DEP ${ARGN})
		target_link_libraries(${LIB} PUBLIC ${DEP})
		target_include_directories(${LIB} PUBLIC "${CMAKE_SOURCE_DIR}/${DEP}")
	endforeach()
endmacro()

macro(copy_headers LIB)
	if(${CMAKE_OS_NAME} STREQUAL "Windows")
		execute_process(COMMAND
			cmd /c call "${CMAKE_SOURCE_DIR}/buildsteps/CopyHeaders.bat"
				"${CMAKE_CURRENT_SOURCE_DIR}/${LIB}"
				"${CMAKE_BINARY_DIR}/Headers/${LIB}"
			WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"

			RESULT_VARIABLE result
			OUTPUT_VARIABLE out
			ERROR_VARIABLE err)
	endif()
endmacro()

macro(copy_binaries EXE)
	if(${CMAKE_OS_NAME} STREQUAL "Windows")
		add_custom_command(
			TARGET ${EXE}
			POST_BUILD
			COMMAND call "${CMAKE_SOURCE_DIR}/buildsteps/CopyBinaries.bat"
						"${CMAKE_SOURCE_DIR}/in/Bin/${CMAKE_OS_NAME}/${CMAKE_PLATFORM_NAME}-${CMAKE_BUILD_TYPE}"
						"${CMAKE_CURRENT_BINARY_DIR}"
						"dll"
			COMMENT "Adds dynamic libraries to the same folder as the exe"
			VERBATIM)
	endif()
endmacro()


enable_testing()


include(FetchContent)

FetchContent_Declare(GoogleTest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG        e2239ee6043f73722e7aa812a459f54a28552929 # release-1.11.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)


FetchContent_MakeAvailable(googletest)

add_subdirectory("DebugTools")
add_subdirectory("Tests")
