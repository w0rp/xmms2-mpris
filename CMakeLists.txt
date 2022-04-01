cmake_minimum_required(VERSION 3.5)
project(xmms2-mpris)

set(SRC_LIST
    src/xmms2-mpris.c
    src/mpris-object.c
    src/mpris-player.c
    src/xmms2.c
    src/mpris.c
    src/art.c
)

set(CMAKE_C_FLAGS "-std=c11 -O3 -Wall -Wno-visibility")

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig)
# This doesn't check for the right package.
pkg_check_modules(XMMS2 REQUIRED xmms2-client>=0.8)
pkg_check_modules(GLIB2 REQUIRED glib-2.0 gio-2.0)
pkg_check_modules(XMMS2GLIB REQUIRED xmms2-client-glib>=0.8)
pkg_check_modules(GIOUNIX gio-unix-2.0)

# Require gdbus-codegen to build this client.
find_program(GDBUSCODEGEN NAMES gdbus-codegen)
if(NOT GDBUSCODEGEN)
    message(SEND_ERROR "Could not find gdbus-codegen")
endif()

# Generate the DBUS mpris main object from the XML interface file.
add_custom_command(OUTPUT src/mpris-object.c src/mpris-object.h
    WORKING_DIRECTORY src
    COMMAND ${GDBUSCODEGEN} --interface-prefix org.mpris. --c-namespace Mpris
        --generate-c-code mpris-object mpris2.xml
    DEPENDS src/mpris2.xml
)

add_custom_command(OUTPUT src/mpris-player.c src/mpris-player.h
    WORKING_DIRECTORY src
    COMMAND ${GDBUSCODEGEN} --interface-prefix org.mpris. --c-namespace Mpris
        --generate-c-code mpris-player mpris2-player.xml
    DEPENDS src/mpris2-player.xml
)

# Include xmms2 stuff
include_directories(SYSTEM ${XMMS2_INCLUDE_DIRS})
link_directories(${XMMS2_LIBRARY_DIRS})
include_directories(${XMMS2GLIB_INCLUDE_DIRS})
link_directories(${XMMS2GLIB_LIBRARY_DIRS})

# Include glib stuff
include_directories(${GIOUNIX_INCLUDE_DIRS})
link_directories(${GIOUNIX_LIBRARY_DIRS})
include_directories(${GLIB2_INCLUDE_DIRS})
link_directories(${GLIB2_LIBRARY_DIRS})

add_executable(xmms2-mpris ${SRC_LIST})
target_link_libraries(xmms2-mpris ${XMMS2_LIBRARIES} ${XMMS2GLIB_LIBRARIES} ${GLIB2_LIBRARIES})

# For installing the binary.
install(TARGETS xmms2-mpris DESTINATION ${CMAKE_INSTALL_PREFIX}/share/xmms2/scripts/startup.d)
