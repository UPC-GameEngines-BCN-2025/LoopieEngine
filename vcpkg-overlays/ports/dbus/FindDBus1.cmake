find_package(PkgConfig QUIET)
pkg_check_modules(DBus1 QUIET dbus-1)

if(NOT DBus1_FOUND)
    find_library(DBus1_LIBRARY NAMES dbus-1)
    find_path(DBus1_INCLUDE_DIR NAMES dbus/dbus.h)
    if(DBus1_LIBRARY AND DBus1_INCLUDE_DIR)
        set(DBus1_FOUND TRUE)
    endif()
endif()

if(DBus1_FOUND)
    add_library(DBus1::DBus1 INTERFACE IMPORTED)
    set_target_properties(DBus1::DBus1 PROPERTIES
        INTERFACE_LINK_LIBRARIES "${DBus1_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${DBus1_INCLUDE_DIRS}"
    )
endif()
