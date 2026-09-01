set(FLYINGDODGEM_SOURCES
    flyingdodgem.c
    fixed96.c
    resources.c
    globals.c
    stubs.c
)

add_executable(FlyingDodgem ${FLYINGDODGEM_SOURCES})

target_include_directories(FlyingDodgem PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(FLYINGDODGEM_PLATFORM_SDL3 AND NOT MSVC_50_FOR_RECCMP)
    target_sources(FlyingDodgem PRIVATE
        platform/sdl3/audio.c
        platform/sdl3/display.c
        platform/sdl3/main.c
        gdi32_compat.c
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
