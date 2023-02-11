# CMakeList.txt : CMake project for ICMP, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

if(WIN32)
	find_library(WSOCK32_LIBRARY wsock32)
    find_library(WS2_32_LIBRARY ws2_32)
endif()

# Add source to this project's executable.
add_executable (ICMP "main.cpp" "ICMP.h" "ping.cpp" "traceroute.cpp")

if(WIN32)
	target_link_libraries(ICMP wsock32 ws2_32)
endif()

# TODO: Add tests and install targets if needed.
