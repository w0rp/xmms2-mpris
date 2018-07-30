cmake_minimum_required(VERSION 3.5)
project(xmms2-mpris)

set(SRC_LIST
    src/xmms2-mpris.c
    src/mpris-object.c
    src/mpris-player.c
)

# Set C version and warning flags for C compilers.
if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "-c11 -Wno-visibility")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "-c11 -Wall -Wno-visibility")
endif()

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig)
# This doesn't check for the right package.
pkg_check_modules(XMMS2 REQUIRED xmms2-client>=0.8)
pkg_check_modules(GLIB2 REQUIRED glib-2.0 gio-2.0)
pkg_check_modules(GIOUNIX gio-unix-2.0)

# Require gdbus-codegen to build this client.
find_program(GDBUSCODEGEN NAMES gdbus-codegen)
if(NOT GDBUSCODEGEN)
    message(SEND_ERROR "Could not find gdbus-codegen")
endif()

# Generate the DBUS mpris main object from the XML interface file.
add_custom_command(OUTPUT src/mpris-object.c
    COMMAND ${GDBUSCODEGEN} --interface-prefix org.mpris. --c-namespace Mpris
        --generate-c-code mpris-object src/mpris2.xml
        --output-directory=src
    DEPENDS src/mpris2.xml
)

add_custom_command(OUTPUT src/mpris-player.c
    COMMAND ${GDBUSCODEGEN} --interface-prefix org.mpris. --c-namespace Mpris
        --generate-c-code mpris-player src/mpris2-player.xml
        --output-directory=src
    DEPENDS src/mpris2-player.xml
)

# Include xmms2 stuff
include_directories(${XMMS2_INCLUDE_DIRS})
link_directories(${XMMS2_LIBRARY_DIRS})

# Include glib stuff
include_directories(${GIOUNIX_INCLUDE_DIRS})
link_directories(${GIOUNIX_LIBRARY_DIRS})
include_directories(${GLIB2_INCLUDE_DIRS})
link_directories(${GLIB2_LIBRARY_DIRS})

add_executable(xmms2-mpris ${SRC_LIST})
target_link_libraries(xmms2-mpris ${XMMS2_LIBRARIES} ${GLIB2_LIBRARIES})
