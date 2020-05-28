cmake_minimum_required(VERSION 2.8.4)
project(Demos)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Config types" FORCE)

add_compile_options(--std=c++14)

if (MSVC)
    add_compile_options("$<$<CONFIG:DEBUG>:/MTd>")
    add_compile_options("$<$<CONFIG:RELEASE>:/MT>")
endif()

function(set_default_definitions TARGET)
    target_compile_definitions(${TARGET} PRIVATE
        HAVE_LIBC
        GLEW_STATIC
        SDL_HAPTIC_DISABLED
        SDL_JOYSTICK_DISABLED
        "$<$<CONFIG:DEBUG>:DEMOS_DEBUG>"
    )
endfunction()

function(set_default_includes TARGET)
    target_include_directories(${TARGET} PRIVATE
        "${CMAKE_SOURCE_DIR}/demos"
        "${CMAKE_SOURCE_DIR}/vendor" # TODO needed?
        "${CMAKE_SOURCE_DIR}/vendor/glew/1.13/include"
        "${CMAKE_SOURCE_DIR}/vendor/SDL/2.0.4/include"
        "${CMAKE_SOURCE_DIR}/vendor/stb_truetype/1.11"
        "${CMAKE_SOURCE_DIR}/vendor/stb_image/2.15"
        "${CMAKE_SOURCE_DIR}/vendor/glm/0.9.8.4"
        "${CMAKE_SOURCE_DIR}/vendor/imgui"
    )
endfunction()

function(add_app TARGET SOURCES)
    file(GLOB SRC ${SOURCES})
    source_group("" FILES ${SRC})
    add_executable(${TARGET} ${SRC})
    set_default_includes(${TARGET})
    set_default_definitions(${TARGET})
    target_link_libraries(${TARGET} Vendor Common)

    if (MSVC)
        target_compile_options(${TARGET} PRIVATE /wd4267 /wd4244 /wd4312)
    endif()
endfunction()

add_subdirectory("demos/common")
add_subdirectory("vendor")

add_app(StbTrueType "${CMAKE_SOURCE_DIR}/demos/stb-truetype/*.cpp;${CMAKE_SOURCE_DIR}/demos/stb-truetype/*.h")
add_app(Transform "${CMAKE_SOURCE_DIR}/demos/transform/*.cpp;${CMAKE_SOURCE_DIR}/demos/transform/*.h")
add_app(Skybox "${CMAKE_SOURCE_DIR}/demos/skybox/*.cpp;${CMAKE_SOURCE_DIR}/demos/skybox/*.h")
add_app(
    ImGui "${CMAKE_SOURCE_DIR}/demos/imgui/*.cpp;${CMAKE_SOURCE_DIR}/vendor/imgui/*.cpp;${CMAKE_SOURCE_DIR}/vendor/imgui/examples/imgui_impl_sdl.cpp;${CMAKE_SOURCE_DIR}/vendor/imgui/examples/imgui_impl_opengl3.cpp"
)
