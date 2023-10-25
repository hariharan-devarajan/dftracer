set(LIBUV_FOUND TRUE)

if (NOT TARGET libuv::libuv)
    # Include directories
    find_path(LIBUV_INCLUDE_DIRS uv.h PATH_SUFFIXES include/)
    if (NOT IS_DIRECTORY "${LIBUV_INCLUDE_DIRS}")
        set(LIBUV_FOUND FALSE)
    endif ()
    message("-- LIBUV_INCLUDE_DIRS: " ${LIBUV_INCLUDE_DIRS})
    get_filename_component(LIBUV_ROOT_DIR ${LIBUV_INCLUDE_DIRS}/.. ABSOLUTE)
    message("-- LIBUV_ROOT_DIR: " ${LIBUV_ROOT_DIR})
    find_path(LIBUV_LIBRARY_PATH libuv.so PATH_SUFFIXES lib64/)
    if (NOT IS_DIRECTORY "${LIBUV_LIBRARY_PATH}")
        find_path(LIBUV_LIBRARY_PATH libuv.so PATH_SUFFIXES lib/)
        if (NOT IS_DIRECTORY "${LIBUV_LIBRARY_PATH}")
            set(LIBUV_FOUND FALSE)
        endif()
    endif ()
    message("-- LIBUV_LIBRARY_PATH: " ${LIBUV_LIBRARY_PATH})
    set(LIBUV_LIBRARIES -L${LIBUV_LIBRARY_PATH} -luv)

    set(LIBUV_DEFINITIONS "")
    add_library(libuv INTERFACE)
    add_library(libuv::libuv ALIAS libuv)
    target_include_directories(libuv INTERFACE ${LIBUV_INCLUDE_DIRS})
    target_link_libraries(libuv INTERFACE ${LIBUV_LIBRARIES})
    target_compile_options(libuv INTERFACE ${LIBUV_DEFINITIONS})

    include(FindPackageHandleStandardArgs)
    # handle the QUIETLY and REQUIRED arguments and set ortools to TRUE
    # if all listed variables are TRUE
    find_package_handle_standard_args(libuv
            REQUIRED_VARS LIBUV_FOUND LIBUV_ROOT_DIR LIBUV_LIBRARIES LIBUV_INCLUDE_DIRS)
endif ()