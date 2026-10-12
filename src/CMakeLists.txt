set(FLYINGDODGEM_SOURCES
    flyingdodgem.c
    fixed96.c
    resources.c
    globals.c
    callees.c
    scene.c
    stubs.c
    texmaze.c
    winmain.c
    pump.c
    render.c
    wallspan.c
    display.c
    app.c
)

# Retail was built with MSVC 4.0 (CL 10.00, 1995). The matching build uses
# that toolchain from the wine prefix (C:/msvc40) and stops without it.
set(FLYINGDODGEM_RECCMP_VC40_SOURCES flyingdodgem.c fixed96.c resources.c globals.c callees.c scene.c stubs.c texmaze.c winmain.c pump.c render.c wallspan.c display.c app.c)

if(MSVC40_FOR_RECCMP AND EXISTS "C:/msvc40/bin/CL.EXE")
    set(_vc_cl "C:/msvc40/bin/CL.EXE")
    set(_vc_inc "C:\\msvc40\\include;C:\\msvc40\\mfc\\include")
    set(_vc_lib "C:\\msvc40\\lib;C:\\msvc40\\mfc\\lib")
    set(_vc_tag "MSVC 4.0")
elseif(MSVC40_FOR_RECCMP)
    message(FATAL_ERROR "MSVC40_FOR_RECCMP requires MSVC 4.0 at C:/msvc40/bin/CL.EXE")
endif()
if(DEFINED _vc_cl)
    set(_vc40_objs "")
    foreach(_f IN LISTS FLYINGDODGEM_RECCMP_VC40_SOURCES)
        set(_obj "${CMAKE_CURRENT_BINARY_DIR}/vc40/${_f}.obj")
        if(_f STREQUAL "winmain.c" OR _f STREQUAL "pump.c" OR _f STREQUAL "render.c" OR _f STREQUAL "wallspan.c" OR _f STREQUAL "scene.c" OR _f STREQUAL "callees.c" OR _f STREQUAL "display.c" OR _f STREQUAL "app.c")
            set(_lang /TP)
        else()
            set(_lang /TC)
        endif()
        # The retail pump frame-omits (sub esp,4 + reg saves, no frame) while
        # WinMain keeps its frame: same MSVC 4 toolchain, /Oy only on pump.
        # scene.c holds CWorld::CWorld (HOVER 0x41e230): retail gives it a
        # full C++ unwinding frame, which MSVC only emits under /GX.
        # render.c likewise holds DrawEntry402250 (HOVER 0x402250), whose
        # case-local CBeaconKey objects force the same /GX prolog in retail.
        if(_f STREQUAL "scene.c")
            set(_extra_flags /GX)
        elseif(_f STREQUAL "pump.c")
            set(_extra_flags /Oy)
        elseif(_f STREQUAL "render.c")
            set(_extra_flags /GX /G4 /Ow /QIfdiv)
        elseif(_f STREQUAL "wallspan.c")
            set(_extra_flags /G4 /Oa)
        elseif(_f STREQUAL "stubs.c")
            # WinHelpA is dllimported: MSVC 4 /G5 saves ebx around
            # the indirect call (push/pop with no use); /G4 drops the pair and
            # matches retail's short thunks instruction for instruction.
            set(_extra_flags /G4)
        else()
            set(_extra_flags "")
        endif()
        # Retail compiled the render TU size-oriented: cmp [mem],0 instead of
        # mov reg,[mem]; test (observed throughout 0x402990), but still
        # frameless like retail. /Os always frames fastcall functions
        # so render.c stays on /O2.
        # (/O1 was tried: it fixes IsKindOf-style epilogue sharing in
        # isolation but regresses the TU badly, including three 100% functions,
        # so it stays off.)
        add_custom_command(OUTPUT "${_obj}"
            COMMAND ${CMAKE_COMMAND} -E env
                "INCLUDE=${_vc_inc}"
                "LIB=${_vc_lib}"
                ${_vc_cl} /nologo /c
                /DWIN32 /D_WINDOWS /Zm1000 /O2 /Ow /Oi /G5 /Zi /ML
                ${_extra_flags}
                /I${CMAKE_CURRENT_SOURCE_DIR} ${_lang}
                /Fo${_obj}
                ${CMAKE_CURRENT_SOURCE_DIR}/${_f}
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_f}
            COMMENT "Compiling ${_f} with ${_vc_tag} (retail match)"
            VERBATIM)
        list(APPEND _vc40_objs "${_obj}")
    endforeach()
    list(REMOVE_ITEM FLYINGDODGEM_SOURCES ${FLYINGDODGEM_RECCMP_VC40_SOURCES})
    list(APPEND FLYINGDODGEM_SOURCES ${_vc40_objs})
endif()

add_executable(FlyingDodgem ${FLYINGDODGEM_SOURCES})

# winmain.c is compiled as C++: the retail entry came from the MFC/C++ side of
# HOVER.EXE and its codegen was validated against that frontend. pump.c likewise
# uses __thiscall member dispatch and needs the C++ frontend.
set_source_files_properties(winmain.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(pump.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(render.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(wallspan.c PROPERTIES LANGUAGE CXX)
# callees.c declares __thiscall-shaped members of a throw-away class, which the
# C frontend rejects, so it too goes through the C++ frontend.  It holds the
# span matching sources (F4030f0/M402e00/F403a60), so it joins the MSVC 4 set
# above when that toolchain is present; the stub bodies are excluded from the
# reccmp statistics and only their address markers matter.
set_source_files_properties(callees.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(scene.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(display.c PROPERTIES LANGUAGE CXX)
# app.c hosts the CBumperApp bootstrap (C++ static initializer, placement
# new into the CBumperDoc CRuntimeClass factory) and must use the C++ frontend.
set_source_files_properties(app.c PROPERTIES LANGUAGE CXX)

target_include_directories(FlyingDodgem PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(FLYINGDODGEM_PLATFORM_SDL3 AND NOT MSVC40_FOR_RECCMP)
    target_sources(FlyingDodgem PRIVATE
        platform/sdl3/audio.c
        platform/sdl3/display.c
        platform/sdl3/main.c
        gdi32_compat.c
        win32_compat.c
        winmm_compat.c
        menu.c
        gameframe.c
        dashhud.c
        display_native.c
    )
    # display_native.c hosts the DisplayEntry worker and uses extern "C" for
    # the matched entry points; it must go through the C++ frontend.
    set_source_files_properties(display_native.c PROPERTIES LANGUAGE CXX)
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
