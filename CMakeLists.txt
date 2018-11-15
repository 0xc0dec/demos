cmake_minimum_required(VERSION 2.8.4)
project(Demos)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Config types" FORCE)

add_compile_options(-std=c++14)

if (MSVC)
    add_compile_options("$<$<CONFIG:DEBUG>:/MTd>")
    add_compile_options("$<$<CONFIG:RELEASE>:/MT>")
endif()


# Demos

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
        "src"
        "vendor" # TODO needed?
        "vendor/glew/1.13/include"
        "vendor/SDL/2.0.4/include"
        "vendor/stb_truetype/1.11"
        "vendor/glm/0.9.8.4"
    )
endfunction()

function(add_exe TARGET ENTRYPOINT)
    source_group("" FILES ${ENTRYPOINT})
    add_executable(${TARGET} ${ENTRYPOINT})
    set_default_includes(${TARGET})
    target_link_libraries(${TARGET} Vendor Common)

    if (MSVC)
        target_compile_options(${TARGET} PRIVATE /wd4267 /wd4244 /wd4312)
    endif()

    set_default_definitions(${TARGET})
endfunction()

function(add_common)
    file(GLOB DEMOS_COMMON "src/common/*.cpp" "src/common/*.h")
    source_group("" FILES ${DEMOS_COMMON})
    add_library(Common ${DEMOS_COMMON})
    set_default_includes(Common)

    if (MSVC)
        target_compile_options(Common PRIVATE /wd4267 /wd4244 /wd4312)
    endif()

    set_default_definitions(Common)
endfunction()

add_common()
add_exe(StbTrueType "src/StbTrueType.cpp")
add_exe(Transform "src/Transform.cpp")


# Vendor

add_subdirectory("vendor")