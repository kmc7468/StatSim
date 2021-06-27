cmake_minimum_required(VERSION 3.15.0)
project(StatSim)

include(CheckIPOSupported)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

include_directories("./include")
file(GLOB_RECURSE SOURCE_LIST "./src/*.cpp")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./bin")

add_executable(${PROJECT_NAME} ${SOURCE_LIST})

set(Boost_INCLUDE_DIR "")
find_package(Boost REQUIRED)
target_link_libraries(StatSim PRIVATE Boost::headers)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	check_ipo_supported(RESULT isIPOSupported)
	if(isIPOSupported)
		set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	endif()
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION "bin")