file(GLOB DEMOS_VENDOR_GLEW_SRC
    "glew/1.13/src/glew.c"
)

file(GLOB DEMOS_VENDOR_IMGUI_SRC
    "imgui/*.cpp"
    "imgui/examples/imgui_impl_opengl3.cpp"
    "imgui/examples/imgui_impl_vulkan.cpp"
    "imgui/examples/imgui_impl_sdl.cpp"
)

source_group("glew" FILES ${DEMOS_VENDOR_GLEW_SRC})
source_group("imgui" FILES ${DEMOS_VENDOR_IMGUI_SRC})

add_library(Vendor STATIC
    ${DEMOS_VENDOR_GLEW_SRC}
    ${DEMOS_VENDOR_IMGUI_SRC}
)

target_include_directories(Vendor PRIVATE
    "."
    "glew/1.13/include"
    "SDL/2.0.12/include"
    "imgui"
    "vulkan/include"
    "vulkan/include/vulkan"
)

set_target_properties(Vendor PROPERTIES FOLDER vendor)

if (WIN32)
    find_library(DEMOS_VULKAN_LIB
        NAMES vulkan-1
        HINTS "vulkan/lib"
    )

    set(DEMOS_VENDOR_PLATFORM_LIBS
        ${DEMOS_VULKAN_LIB}
        winmm.lib
        imm32.lib
        version.lib
    )
elseif(APPLE)
    find_library(DEMOS_VENDOR_CARBON_LIB Carbon)
    find_library(DEMOS_VENDOR_IOKIT_LIB IOKit)
    find_library(DEMOS_VENDOR_FORCEFEEDBACK_LIB ForceFeedback)
    find_library(DEMOS_VENDOR_COCOA_LIB Cocoa)
    find_library(DEMOS_VENDOR_AUDIOUNIT_LIB AudioUnit)
    find_library(DEMOS_VENDOR_COREAUDIO_LIB CoreAudio)
    find_library(DEMOS_VENDOR_COREVIDEO_LIB CoreVideo)
    find_library(DEMOS_VENDOR_COREFOUNDATION_LIB CoreFoundation)

    set(DEMOS_VENDOR_PLATFORM_LIBS
        ${DEMOS_VENDOR_CARBON_LIB}
        ${DEMOS_VENDOR_IOKIT_LIB}
        ${DEMOS_VENDOR_FORCEFEEDBACK_LIB}
        ${DEMOS_VENDOR_COCOA_LIB}
        ${DEMOS_VENDOR_AUDIOUNIT_LIB}
        ${DEMOS_VENDOR_COREAUDIO_LIB}
        ${DEMOS_VENDOR_COREVIDEO_LIB}
        ${DEMOS_VENDOR_COREFOUNDATION_LIB}
    )
endif()

find_package(OpenGL REQUIRED)

target_link_libraries(Vendor ${OPENGL_LIBRARY} ${DEMOS_VENDOR_PLATFORM_LIBS} SDL2-static)
set_default_definitions(Vendor)


# For SDL
option(FORCE_STATIC_VCRT on)

add_subdirectory("SDL/2.0.12")
set_target_properties(SDL2 PROPERTIES FOLDER vendor)
set_target_properties(SDL2main PROPERTIES FOLDER vendor)
set_target_properties(SDL2-static PROPERTIES FOLDER vendor)