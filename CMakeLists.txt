cmake_minimum_required(VERSION 2.8.4)
project(Demos)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Config types" FORCE)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (MSVC)
    add_compile_options("$<$<CONFIG:DEBUG>:/MTd>")
    add_compile_options("$<$<CONFIG:RELEASE>:/MT>")
    add_compile_options(/std:c++14)
    add_compile_options(/MP)
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
        "${CMAKE_SOURCE_DIR}/vendor/vulkan/include"
        "${CMAKE_SOURCE_DIR}/vendor/vulkan/include/vulkan"
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

add_subdirectory("vendor")
add_subdirectory("demos/common")
add_subdirectory("demos/stb-truetype")
add_subdirectory("demos/transform")
add_subdirectory("demos/skybox")
add_subdirectory("demos/imgui")
add_subdirectory("demos/vulkan")

