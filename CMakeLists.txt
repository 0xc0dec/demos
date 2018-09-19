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

file(GLOB DEMOS_COMMON "src/common/*.cpp" "src/common/*.h")

source_group("common" FILES ${DEMOS_COMMON})

function(set_default_definitions NAME)
    target_compile_definitions(${NAME} PRIVATE
        HAVE_LIBC
        GLEW_STATIC
        SDL_HAPTIC_DISABLED
        SDL_JOYSTICK_DISABLED
        "$<$<CONFIG:DEBUG>:DEMOS_DEBUG>"
    )
endfunction()

function(add_exe NAME ENTRYPOINT)
    add_executable(${NAME} ${DEMOS_COMMON} ${ENTRYPOINT})

    target_include_directories(${NAME} PRIVATE
        "src"
        "vendor" # TODO needed?
        "vendor/glew/1.13/include"
        "vendor/SDL/2.0.4/include"
        "vendor/stb_truetype/1.11"
        "vendor/glm/0.9.8.4"
    )

    target_link_libraries(${NAME} Vendor)

    if (MSVC)
        target_compile_options(${NAME} PRIVATE /wd4267 /wd4244 /wd4312)
    endif()

    set_default_definitions(${NAME})
endfunction()

add_exe(StbTrueType "src/StbTrueType.cpp")
add_exe(Transform "src/Transform.cpp")


# Vendor

file(GLOB DEMOS_VENDOR_GLEW_SRC
    "vendor/glew/1.13/src/glew.c"
)
 
if (WIN32)
    file(GLOB DEMOS_VENDOR_SDL_SRC
        "vendor/SDL/2.0.4/src/haptic/dummy/SDL_syshaptic.c"
        "vendor/SDL/2.0.4/src/joystick/dummy/SDL_sysjoystick.c"
        "vendor/SDL/2.0.4/src/joystick/SDL_gamecontroller.c"
        "vendor/SDL/2.0.4/src/joystick/SDL_joystick.c"
        "vendor/SDL/2.0.4/src/libm/e_atan2.c"
        "vendor/SDL/2.0.4/src/libm/e_log.c"
        "vendor/SDL/2.0.4/src/libm/e_pow.c"
        "vendor/SDL/2.0.4/src/libm/e_rem_pio2.c"
        "vendor/SDL/2.0.4/src/libm/e_sqrt.c"
        "vendor/SDL/2.0.4/src/libm/k_cos.c"
        "vendor/SDL/2.0.4/src/libm/k_rem_pio2.c"
        "vendor/SDL/2.0.4/src/libm/k_sin.c"
        "vendor/SDL/2.0.4/src/libm/k_tan.c"
        "vendor/SDL/2.0.4/src/libm/s_atan.c"
        "vendor/SDL/2.0.4/src/libm/s_copysign.c"
        "vendor/SDL/2.0.4/src/libm/s_cos.c"
        "vendor/SDL/2.0.4/src/libm/s_fabs.c"
        "vendor/SDL/2.0.4/src/libm/s_floor.c"
        "vendor/SDL/2.0.4/src/libm/s_scalbn.c"
        "vendor/SDL/2.0.4/src/libm/s_sin.c"
        "vendor/SDL/2.0.4/src/libm/s_tan.c"
        "vendor/SDL/2.0.4/src/SDL.c"
        "vendor/SDL/2.0.4/src/SDL_assert.c"
        "vendor/SDL/2.0.4/src/atomic/SDL_atomic.c"
        "vendor/SDL/2.0.4/src/audio/SDL_audio.c"
        "vendor/SDL/2.0.4/src/audio/SDL_audiocvt.c"
        "vendor/SDL/2.0.4/src/audio/SDL_audiodev.c"
        "vendor/SDL/2.0.4/src/audio/SDL_audiotypecvt.c"
        "vendor/SDL/2.0.4/src/render/software/SDL_blendfillrect.c"
        "vendor/SDL/2.0.4/src/render/software/SDL_blendline.c"
        "vendor/SDL/2.0.4/src/render/software/SDL_blendpoint.c"
        "vendor/SDL/2.0.4/src/video/SDL_blit.c"
        "vendor/SDL/2.0.4/src/video/SDL_blit_0.c"
        "vendor/SDL/2.0.4/src/video/SDL_blit_1.c"
        "vendor/SDL/2.0.4/src/video/SDL_blit_A.c"
        "vendor/SDL/2.0.4/src/video/SDL_blit_auto.c"
        "vendor/SDL/2.0.4/src/video/SDL_blit_copy.c"
        "vendor/SDL/2.0.4/src/video/SDL_blit_N.c"
        "vendor/SDL/2.0.4/src/video/SDL_blit_slow.c"
        "vendor/SDL/2.0.4/src/video/SDL_bmp.c"
        "vendor/SDL/2.0.4/src/video/SDL_clipboard.c"
        "vendor/SDL/2.0.4/src/events/SDL_clipboardevents.c"
        "vendor/SDL/2.0.4/src/cpuinfo/SDL_cpuinfo.c"
        "vendor/SDL/2.0.4/src/render/SDL_d3dmath.c"
        "vendor/SDL/2.0.4/src/haptic/windows/SDL_dinputhaptic.c"
        "vendor/SDL/2.0.4/src/joystick/windows/SDL_dinputjoystick.c"
        "vendor/SDL/2.0.4/src/audio/directsound/SDL_directsound.c"
        "vendor/SDL/2.0.4/src/audio/disk/SDL_diskaudio.c"
        "vendor/SDL/2.0.4/src/render/software/SDL_drawline.c"
        "vendor/SDL/2.0.4/src/render/software/SDL_drawpoint.c"
        "vendor/SDL/2.0.4/src/events/SDL_dropevents.c"
        "vendor/SDL/2.0.4/src/audio/dummy/SDL_dummyaudio.c"
        "vendor/SDL/2.0.4/src/dynapi/SDL_dynapi.c"
        "vendor/SDL/2.0.4/src/video/SDL_egl.c"
        "vendor/SDL/2.0.4/src/SDL_error.c"
        "vendor/SDL/2.0.4/src/events/SDL_events.c"
        "vendor/SDL/2.0.4/src/video/SDL_fillrect.c"
        "vendor/SDL/2.0.4/src/events/SDL_gesture.c"
        "vendor/SDL/2.0.4/src/stdlib/SDL_getenv.c"
        "vendor/SDL/2.0.4/src/haptic/SDL_haptic.c"
        "vendor/SDL/2.0.4/src/SDL_hints.c"
        "vendor/SDL/2.0.4/src/stdlib/SDL_iconv.c"
        "vendor/SDL/2.0.4/src/events/SDL_keyboard.c"
        "vendor/SDL/2.0.4/src/SDL_log.c"
        "vendor/SDL/2.0.4/src/stdlib/SDL_malloc.c"
        "vendor/SDL/2.0.4/src/audio/SDL_mixer.c"
        "vendor/SDL/2.0.4/src/joystick/windows/SDL_mmjoystick.c"
        "vendor/SDL/2.0.4/src/events/SDL_mouse.c"
        "vendor/SDL/2.0.4/src/video/dummy/SDL_nullevents.c"
        "vendor/SDL/2.0.4/src/video/dummy/SDL_nullframebuffer.c"
        "vendor/SDL/2.0.4/src/video/dummy/SDL_nullvideo.c"
        "vendor/SDL/2.0.4/src/video/SDL_pixels.c"
        "vendor/SDL/2.0.4/src/power/SDL_power.c"
        "vendor/SDL/2.0.4/src/stdlib/SDL_qsort.c"
        "vendor/SDL/2.0.4/src/events/SDL_quit.c"
        "vendor/SDL/2.0.4/src/video/SDL_rect.c"
        "vendor/SDL/2.0.4/src/render/SDL_render.c"
        "vendor/SDL/2.0.4/src/render/direct3d/SDL_render_d3d.c"
        "vendor/SDL/2.0.4/src/render/direct3d11/SDL_render_d3d11.c"
        "vendor/SDL/2.0.4/src/render/opengl/SDL_render_gl.c"
        "vendor/SDL/2.0.4/src/render/opengles2/SDL_render_gles2.c"
        "vendor/SDL/2.0.4/src/render/software/SDL_render_sw.c"
        "vendor/SDL/2.0.4/src/video/SDL_RLEaccel.c"
        "vendor/SDL/2.0.4/src/render/software/SDL_rotate.c"
        "vendor/SDL/2.0.4/src/file/SDL_rwops.c"
        "vendor/SDL/2.0.4/src/render/opengl/SDL_shaders_gl.c"
        "vendor/SDL/2.0.4/src/render/opengles2/SDL_shaders_gles2.c"
        "vendor/SDL/2.0.4/src/video/SDL_shape.c"
        "vendor/SDL/2.0.4/src/atomic/SDL_spinlock.c"
        "vendor/SDL/2.0.4/src/stdlib/SDL_stdlib.c"
        "vendor/SDL/2.0.4/src/video/SDL_stretch.c"
        "vendor/SDL/2.0.4/src/stdlib/SDL_string.c"
        "vendor/SDL/2.0.4/src/video/SDL_surface.c"
        "vendor/SDL/2.0.4/src/thread/generic/SDL_syscond.c"
        "vendor/SDL/2.0.4/src/filesystem/windows/SDL_sysfilesystem.c"
        "vendor/SDL/2.0.4/src/loadso/windows/SDL_sysloadso.c"
        "vendor/SDL/2.0.4/src/thread/windows/SDL_sysmutex.c"
        "vendor/SDL/2.0.4/src/power/windows/SDL_syspower.c"
        "vendor/SDL/2.0.4/src/thread/windows/SDL_syssem.c"
        "vendor/SDL/2.0.4/src/thread/windows/SDL_systhread.c"
        "vendor/SDL/2.0.4/src/timer/windows/SDL_systimer.c"
        "vendor/SDL/2.0.4/src/thread/windows/SDL_systls.c"
        "vendor/SDL/2.0.4/src/thread/SDL_thread.c"
        "vendor/SDL/2.0.4/src/timer/SDL_timer.c"
        "vendor/SDL/2.0.4/src/events/SDL_touch.c"
        "vendor/SDL/2.0.4/src/video/SDL_video.c"
        "vendor/SDL/2.0.4/src/audio/SDL_wave.c"
        "vendor/SDL/2.0.4/src/events/SDL_windowevents.c"
        "vendor/SDL/2.0.4/src/core/windows/SDL_windows.c"
        "vendor/SDL/2.0.4/src/video/windows/SDL_windowsclipboard.c"
        "vendor/SDL/2.0.4/src/video/windows/SDL_windowsevents.c"
        "vendor/SDL/2.0.4/src/video/windows/SDL_windowsframebuffer.c"
        "vendor/SDL/2.0.4/src/video/windows/SDL_windowskeyboard.c"
        "vendor/SDL/2.0.4/src/video/windows/SDL_windowsmessagebox.c"
        "vendor/SDL/2.0.4/src/video/windows/SDL_windowsmodes.c"
        "vendor/SDL/2.0.4/src/video/windows/SDL_windowsmouse.c"
        "vendor/SDL/2.0.4/src/video/windows/SDL_windowsopengl.c"
        "vendor/SDL/2.0.4/src/video/windows/SDL_windowsopengles.c"
        "vendor/SDL/2.0.4/src/video/windows/SDL_windowsshape.c"
        "vendor/SDL/2.0.4/src/video/windows/SDL_windowsvideo.c"
        "vendor/SDL/2.0.4/src/video/windows/SDL_windowswindow.c"
        "vendor/SDL/2.0.4/src/audio/winmm/SDL_winmm.c"
        "vendor/SDL/2.0.4/src/audio/xaudio2/SDL_xaudio2.c"
        "vendor/SDL/2.0.4/src/core/windows/SDL_xinput.c"
        "vendor/SDL/2.0.4/src/render/SDL_yuv_mmx.c"
        "vendor/SDL/2.0.4/src/render/SDL_yuv_sw.c"
    )
elseif(APPLE)
    file(GLOB DEMOS_VENDOR_SDL_SRC
        "vendor/SDL/2.0.4/src/*.c"
        "vendor/SDL/2.0.4/src/atomic/*.c"
        "vendor/SDL/2.0.4/src/audio/*.c"
        "vendor/SDL/2.0.4/src/audio/disk/*.c"
        "vendor/SDL/2.0.4/src/audio/dummy/*.c"
        "vendor/SDL/2.0.4/src/audio/coreaudio/*.c"
        "vendor/SDL/2.0.4/src/cpuinfo/*.c"
        "vendor/SDL/2.0.4/src/dynapi/*.c"
        "vendor/SDL/2.0.4/src/events/*.c"
        "vendor/SDL/2.0.4/src/filesystem/cocoa/*.m"
        "vendor/SDL/2.0.4/src/file/*.c"
        "vendor/SDL/2.0.4/src/file/cocoa/*.m"
        "vendor/SDL/2.0.4/src/haptic/*.c"
        "vendor/SDL/2.0.4/src/haptic/darwin/*.c"
        "vendor/SDL/2.0.4/src/joystick/*.c"
        "vendor/SDL/2.0.4/src/joystick/darwin/*.c"
        "vendor/SDL/2.0.4/src/loadso/dlopen/*.c"
        "vendor/SDL/2.0.4/src/power/*.c"
        "vendor/SDL/2.0.4/src/power/macosx/*.c"
        "vendor/SDL/2.0.4/src/render/*.c"
        "vendor/SDL/2.0.4/src/render/opengl/*.c"
        "vendor/SDL/2.0.4/src/render/software/*.c"
        "vendor/SDL/2.0.4/src/stdlib/*.c"
        "vendor/SDL/2.0.4/src/thread/*.c"
        "vendor/SDL/2.0.4/src/thread/pthread/*.c"
        "vendor/SDL/2.0.4/src/timer/*.c"
        "vendor/SDL/2.0.4/src/timer/unix/*.c"
        "vendor/SDL/2.0.4/src/video/*.c"
        "vendor/SDL/2.0.4/src/video/cocoa/*.m"
        "vendor/SDL/2.0.4/src/video/dummy/*.c"
        "vendor/SDL/2.0.4/src/video/x11/*.c"
    )
endif()

add_library(Vendor STATIC ${DEMOS_VENDOR_GLEW_SRC} ${DEMOS_VENDOR_SDL_SRC})

source_group("glew" FILES ${DEMOS_VENDOR_GLEW_SRC})
source_group("SDL" FILES ${DEMOS_VENDOR_SDL_SRC})

target_include_directories(Vendor PRIVATE
    "vendor"
    "vendor/glew/1.13/include"
    "vendor/SDL/2.0.4/include"
)

if (APPLE)
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

if (WIN32)
    set(DEMOS_VENDOR_PLATFORM_LIBS
        winmm.lib
        imm32.lib
        version.lib
    )
endif()

find_package(OpenGL REQUIRED)

target_link_libraries(Vendor ${OPENGL_LIBRARY} ${DEMOS_VENDOR_PLATFORM_LIBS})

set_default_definitions(Vendor)
