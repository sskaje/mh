cmake_minimum_required(VERSION 3.7)

project(mh)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

if (NOT ${APPLE})
    message( FATAL_ERROR "OSX only" )
endif()

if ("$ENV{BUILD_ARCH}")
    set(BUILD_ARCH "$ENV{BUILD_ARCH}")
endif ()

if ("$ENV{IOS_JAILBREAK_ELECTRA}")
    set(IOS_JAILBREAK_ELECTRA "$ENV{IOS_JAILBREAK_ELECTRA}")
endif()

if ("$ENV{LOCAL_SDK_ROOT}")
    set(LOCAL_SDK_ROOT "$ENV{LOCAL_SDK_ROOT}")
endif()


if (EXISTS ${LOCAL_SDK_ROOT})
    set(SDK_ROOT ${LOCAL_SDK_ROOT})
else()
    set(SDK_ROOT "/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs")
endif()


execute_process(COMMAND echo "SDK_ROOT=${LOCAL_SDK_ROOT}")
execute_process(COMMAND echo "BUILD_ARCH=${BUILD_ARCH}")


if ("${BUILD_ARCH}" MATCHES "armv7s")
    set(CMAKE_OSX_SYSROOT "${SDK_ROOT}/iPhoneOS10.3.sdk")
    set(CMAKE_OSX_ARCHITECTURES "${BUILD_ARCH}")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTARGET_OS_IPHONE=1")
elseif ("${BUILD_ARCH}" MATCHES "armv7")
    set(CMAKE_OSX_SYSROOT "${SDK_ROOT}/iPhoneOS10.3.sdk")
    set(CMAKE_OSX_ARCHITECTURES "${BUILD_ARCH}")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTARGET_OS_IPHONE=1")
elseif ("${BUILD_ARCH}" MATCHES "arm64")
    set(CMAKE_OSX_SYSROOT "${SDK_ROOT}/iPhoneOS11.2.sdk")
    set(CMAKE_OSX_ARCHITECTURES "${BUILD_ARCH}")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTARGET_OS_IPHONE=1")

    if ("${IOS_JAILBREAK_ELECTRA}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DIOS_JAILBREAK_ELECTRA=1")
        set(MISC_FILES
                ${CMAKE_SOURCE_DIR}/src/app/misc/jailbreak.c
                )

        execute_process(COMMAND echo "Jailbreak!")
    endif()
else ()
    link_directories(/usr/lib)
endif ()

include_directories(src)
include_directories(${CMAKE_SOURCE_DIR}/deps/linenoise)
include_directories(${CMAKE_SOURCE_DIR}/deps/duktape)

set(LINENOISE_FILES
        ${CMAKE_SOURCE_DIR}/deps/linenoise/linenoise.c
        )

if(EXISTS ${CMAKE_SOURCE_DIR}/deps/duktape/duktape.c)
    set(DUKTAPE_FILES
        ${CMAKE_SOURCE_DIR}/deps/duktape/duk_config.h
        ${CMAKE_SOURCE_DIR}/deps/duktape/duktape.h
        ${CMAKE_SOURCE_DIR}/deps/duktape/duktape.c
        ${CMAKE_SOURCE_DIR}/deps/duktape/extras/console/duk_console.c
        )

    file(GLOB SCRIPT_FUNCTION_FILES ${CMAKE_SOURCE_DIR}/src/app/scripts/*.c)

    set(SCRIPT_FILES
        ${SCRIPT_FUNCTION_FILES}
        ${CMAKE_SOURCE_DIR}/src/app/scripts.c
        ${CMAKE_SOURCE_DIR}/src/app/commands/script.c
        )

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DJAVASCRIPT_SUPPORT=1")
endif()

set(LIBRARY_FILES
        ${CMAKE_SOURCE_DIR}/src/mh/mh.c
        ${CMAKE_SOURCE_DIR}/src/mh/search.c
        ${CMAKE_SOURCE_DIR}/src/mh/utils.c
        ${CMAKE_SOURCE_DIR}/src/mh/result.c
        )


#set(DEPENDENCY_FILES)

set(APP_FILES
        ${CMAKE_SOURCE_DIR}/src/app/config.h
        ${CMAKE_SOURCE_DIR}/src/app/app.h
        ${CMAKE_SOURCE_DIR}/src/app/misc.h
        ${CMAKE_SOURCE_DIR}/src/app/context.c
        
        ${CMAKE_SOURCE_DIR}/src/app/commands/commands.h
        ${CMAKE_SOURCE_DIR}/src/app/commands/help.c
        ${CMAKE_SOURCE_DIR}/src/app/commands/memory.c
        ${CMAKE_SOURCE_DIR}/src/app/commands/search.c
        ${CMAKE_SOURCE_DIR}/src/app/commands/open.c
        ${CMAKE_SOURCE_DIR}/src/app/commands/utils.c
        ${CMAKE_SOURCE_DIR}/src/app/commands/process.c

        ${MISC_FILES}

        ${SCRIPT_FILES}
        )

add_executable(mh_cli
        ${CMAKE_SOURCE_DIR}/src/app/args.c
        ${CMAKE_SOURCE_DIR}/src/app/main.c
        ${APP_FILES}
        ${LIBRARY_FILES}
        ${DEPENDENCY_FILES}
        ${LINENOISE_FILES}
        ${DUKTAPE_FILES}
        )

add_executable(mh_script
        ${CMAKE_SOURCE_DIR}/src/app/script-main.c
        ${APP_FILES}
        ${LIBRARY_FILES}
        ${DEPENDENCY_FILES}
        ${LINENOISE_FILES}
        ${DUKTAPE_FILES}
        )


add_subdirectory(tests)

