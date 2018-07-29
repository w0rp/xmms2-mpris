cmake_minimum_required(VERSION 3.5)
project(xmms2-mpris)

if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "-c11 -Wno-visibility")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "-c11 -Wall -Wno-visibility")
endif()

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Require xmms2 tools to be installed.
# This doesn't check for the right package.
find_package(PkgConfig)
pkg_check_modules(XMMS2 REQUIRED xmms2-client>=0.8)

include_directories(/usr/include/xmms2)
add_executable(xmms2-mpris src/xmms2-mpris.c)
target_link_libraries(xmms2-mpris xmmsclient)
