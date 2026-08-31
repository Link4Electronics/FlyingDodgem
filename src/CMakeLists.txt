set(FLYINGDODGEM_SOURCES
    flyingdodgem.c
    fixed96.c
)

add_executable(FlyingDodgem ${FLYINGDODGEM_SOURCES})

target_include_directories(FlyingDodgem PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(FLYINGDODGEM_PLATFORM_SDL3 AND NOT MSVC_50_FOR_RECCMP)
    target_sources(FlyingDodgem PRIVATE
        platform/sdl3/audio.c
        platform/sdl3/main.c
    )
    target_link_libraries(FlyingDodgem PRIVATE SDL3::SDL3)
else()
    target_sources(FlyingDodgem PRIVATE
        platform/null/audio.c
    )
endif()
