set(FLYINGDODGEM_SOURCES
    flyingdodgem.c
    fixed96.c
    resources.c
    globals.c
    stubs.c
    texmaze.c
    winmain.c
    pump.c
    render.c
    wallspan.c
)

# Retail codegen for these TUs is closer to MSVC 4.2 than MSVC 5.0 (WinMain
# and the fixed96 helpers both jump several points when compiled with 10.20),
# so when a VC4.2 toolchain is present in the wine prefix they are compiled
# separately and the rest keeps the MSVC 5.0 rules. CI, which only has MSVC
# 5.0, is unaffected because of the EXISTS() guard.
set(FLYINGDODGEM_RECCMP_VC42_SOURCES fixed96.c winmain.c pump.c render.c wallspan.c)

if(MSVC_50_FOR_RECCMP AND EXISTS "C:/msvc/bin/CL.EXE")
    set(_vc42_objs "")
    foreach(_f IN LISTS FLYINGDODGEM_RECCMP_VC42_SOURCES)
        set(_obj "${CMAKE_CURRENT_BINARY_DIR}/vc42/${_f}.obj")
        if(_f STREQUAL "winmain.c" OR _f STREQUAL "pump.c" OR _f STREQUAL "render.c" OR _f STREQUAL "wallspan.c")
            set(_lang /TP)
        else()
            set(_lang /TC)
        endif()
        # The retail pump frame-omits (sub esp,4 + reg saves, no frame) while
        # WinMain keeps its frame: same VC4.2 toolchain, /Oy only on pump.
        if(_f STREQUAL "pump.c")
            set(_extra_flags /Oy)
        elseif(_f STREQUAL "render.c")
            set(_extra_flags /G4)
        elseif(_f STREQUAL "wallspan.c")
            set(_extra_flags /G4)
        else()
            set(_extra_flags "")
        endif()
        # Retail compiled the render TU size-oriented: cmp [mem],0 instead of
        # mov reg,[mem]; test (observed throughout 0x402990), but still
        # frameless like retail. VC4.2 /Os always frames fastcall functions
        # and VC5 /O1 loses too much shape, so render.c stays on VC4.2 /O2.
        add_custom_command(OUTPUT "${_obj}"
            COMMAND ${CMAKE_COMMAND} -E env
                "INCLUDE=C:\\msvc\\include;C:\\msvc\\mfc\\include"
                "LIB=C:\\msvc\\lib;C:\\msvc\\mfc\\lib"
                C:/msvc/bin/CL.EXE /nologo /c
                /DWIN32 /D_WINDOWS /Zm1000 /O2 /Oi /G5 /Zi /ML
                ${_extra_flags}
                /I${CMAKE_CURRENT_SOURCE_DIR} ${_lang}
                /Fo${_obj}
                ${CMAKE_CURRENT_SOURCE_DIR}/${_f}
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_f}
            COMMENT "Compiling ${_f} with MSVC 4.2 (retail match)"
            VERBATIM)
        list(APPEND _vc42_objs "${_obj}")
    endforeach()
    list(REMOVE_ITEM FLYINGDODGEM_SOURCES ${FLYINGDODGEM_RECCMP_VC42_SOURCES})
    list(APPEND FLYINGDODGEM_SOURCES ${_vc42_objs})
endif()

add_executable(FlyingDodgem ${FLYINGDODGEM_SOURCES})

# winmain.c is compiled as C++: the retail entry came from the MFC/C++ side of
# HOVER.EXE and its codegen was validated against that frontend. pump.c likewise
# uses __thiscall member dispatch and needs the C++ frontend.
set_source_files_properties(winmain.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(pump.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(render.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(wallspan.c PROPERTIES LANGUAGE CXX)

target_include_directories(FlyingDodgem PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(FLYINGDODGEM_PLATFORM_SDL3 AND NOT MSVC_50_FOR_RECCMP)
    target_sources(FlyingDodgem PRIVATE
        platform/sdl3/audio.c
        platform/sdl3/display.c
        platform/sdl3/main.c
        gdi32_compat.c
        win32_compat.c
        menu.c
        gameframe.c
        dashhud.c
    )
    target_compile_definitions(FlyingDodgem PRIVATE FLYINGDODGEM_GDI_COMPAT=1)
    target_link_libraries(FlyingDodgem PRIVATE SDL3::SDL3)

    # Ship the dashboard art (resource-only DLL assets, see AGENTS.md) next to
    # the binary so the GDI-compat resource loader can find it at runtime.
    if(EXISTS "${CMAKE_SOURCE_DIR}/build/dashres")
        add_custom_command(TARGET FlyingDodgem POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/build/dashres"
                "$<TARGET_FILE_DIR:FlyingDodgem>/dashres")
    endif()
else()
    target_sources(FlyingDodgem PRIVATE
        platform/null/audio.c
        platform/null/display.c
    )
endif()
