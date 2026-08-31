add_executable(FlyingDodgem flyingdodgem.c)

if(FLYINGDODGEM_PLATFORM_SDL3 AND NOT MSVC_50_FOR_RECCMP)
    target_link_libraries(FlyingDodgem PRIVATE SDL3::SDL3)
endif()