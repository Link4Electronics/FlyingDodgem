cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo" FORCE)
endif()

project(FlyingDodgem C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
include(FetchContent)

if(MSVC)
    set(CMAKE_INSTALL_BINDIR "." CACHE PATH "User executables")
    set(CMAKE_INSTALL_DOCDIR "." CACHE PATH "Documentation root")
endif()

include(GNUInstallDirs)
include(CheckCCompilerFlag)
include(CMakeDependentOption)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" FLYINGDODGEM_VERSION)
    string(STRIP "${FLYINGDODGEM_VERSION}" FLYINGDODGEM_VERSION)
else()
    set(FLYINGDODGEM_VERSION "unknown")
endif()

message(STATUS "FlyingDodgem version ${FLYINGDODGEM_VERSION}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

set(FLYINGDODGEM_IDE_ARGUMENTS "" CACHE STRING "FlyingDodgem arguments (only used by MSVC when debugging)")
set(FLYINGDODGEM_IDE_ROOT_DIR "" CACHE PATH "FlyingDodgem rootdir (only used by MSVC when debugging)")
option(BUILD_TESTS "Build unit tests." OFF)
option(FLYINGDODGEM_INSTALL "Add install target" OFF)
option(FLYINGDODGEM_WERROR "Treat warnings as errors")
option(FLYINGDODGEM_FIX_BUGS "Fix FlyingDodgem bugs" ON)
option(FLYINGDODGEM_PLATFORM_SDL3 "Support SDL 3 platform driver" ON)

option(MSVC_50_FOR_RECCMP "Build with MSVC 5.0 to match assembly" OFF)

if(MSVC_50_FOR_RECCMP)
    set(FLYINGDODGEM_FIX_BUGS OFF)
    set(BUILD_TESTS OFF)
    set(FLYINGDODGEM_PLATFORM_SDL3 OFF)

    set(CMAKE_MSVC_RUNTIME_LIBRARY "")

    # HOVER.EXE was shipped with MSVC 5.0 at /O2 optimization. We compile the
    # matching (reccmp) build with the same optimization but retain /Zi debug
    # info so reccmp can resolve function symbols from the PDB.
    set(CMAKE_C_FLAGS_DEBUG "/O2 /Oi /G5 /Zi /ML")
    set(CMAKE_C_FLAGS_RELEASE "/O2 /Ob2 /G5 /DNDEBUG /ML")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "/O2 /Ob2 /G5 /DNDEBUG /Zi /ML")
    set(CMAKE_C_FLAGS_MINSIZEREL "/Os /DNDEBUG /ML")
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL}")

    set(CMAKE_EXE_LINKER_FLAGS "/machine:I386")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/incremental:yes /debug")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/incremental:no")
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/incremental:no /debug")
    set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "/incremental:no")

    include("cmake/reccmp.cmake")
endif()

if(FLYINGDODGEM_PLATFORM_SDL3)
    find_package(SDL3 CONFIG REQUIRED)
endif()

if(NOT FLYINGDODGEM_PLATFORM_SDL3 AND NOT MSVC_50_FOR_RECCMP)
    message(FATAL_ERROR "No platform drivers enabled")
endif()

add_subdirectory(src)

if(FLYINGDODGEM_WERROR)
    if(MSVC)
        target_compile_options(compile_with_werror INTERFACE /WX)
    else()
        target_compile_options(compile_with_werror INTERFACE -Werror)
    endif()
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

if(MSVC_50_FOR_RECCMP)
    reccmp_add_target(FlyingDodgem ID HOVER)
    reccmp_configure()
endif()

if(FLYINGDODGEM_INSTALL)
    if(APPLE)
        install(FILES LICENSE DESTINATION ".")
    else()
        install(FILES LICENSE DESTINATION "${CMAKE_INSTALL_DOCDIR}")
    endif()
endif()
