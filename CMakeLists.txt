cmake_minimum_required(VERSION 2.8)
project(bilionaire)

message("config types: ${CMAKE_CONFIGURATION_TYPES}")

set(CMAKE_BUILD_TYPE Release)
cmake_policy(SET CMP0015 NEW)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -pie -fPIE -Wl,--whole-archive -Wl,--no-whole-archive")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++11 -O0 -g")

set(PYTHON_V2 OFF)

if(${PYTHON_V2})
add_definitions( -DPYTHON2 )
include_directories(
    include
    /usr/include/python2.7
)
else(${PYTHON_V2})
include_directories(
    include
    /usr/include/python3.7
)
endif(${PYTHON_V2})

LINK_DIRECTORIES(
    lib
    /usr/local/lib
)

set(SOURCE
    src/devcom.cpp
)

set(PY_SOURCE
    src/devcom.cpp
    src/hobotx2module.cpp
)

add_library(ggcdevcom SHARED ${SOURCE})

add_library(hobotx2 SHARED ${PY_SOURCE})
if(${PYTHON_V2})
target_link_libraries(hobotx2 python2.7 _hbipc_ap)
else(${PYTHON_V2})
target_link_libraries(hobotx2 python3.7m _hbipc_ap)
endif(${PYTHON_V2})

