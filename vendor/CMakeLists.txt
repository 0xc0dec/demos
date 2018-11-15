file(GLOB DEMOS_VENDOR_GLEW_SRC
    "glew/1.13/src/glew.c"
)
 
if (WIN32)
    file(GLOB DEMOS_VENDOR_SDL_SRC
        "SDL/2.0.4/src/haptic/dummy/SDL_syshaptic.c"
        "SDL/2.0.4/src/joystick/dummy/SDL_sysjoystick.c"
        "SDL/2.0.4/src/joystick/SDL_gamecontroller.c"
        "SDL/2.0.4/src/joystick/SDL_joystick.c"
        "SDL/2.0.4/src/libm/e_atan2.c"
        "SDL/2.0.4/src/libm/e_log.c"
        "SDL/2.0.4/src/libm/e_pow.c"
        "SDL/2.0.4/src/libm/e_rem_pio2.c"
        "SDL/2.0.4/src/libm/e_sqrt.c"
        "SDL/2.0.4/src/libm/k_cos.c"
        "SDL/2.0.4/src/libm/k_rem_pio2.c"
        "SDL/2.0.4/src/libm/k_sin.c"
        "SDL/2.0.4/src/libm/k_tan.c"
        "SDL/2.0.4/src/libm/s_atan.c"
        "SDL/2.0.4/src/libm/s_copysign.c"
        "SDL/2.0.4/src/libm/s_cos.c"
        "SDL/2.0.4/src/libm/s_fabs.c"
        "SDL/2.0.4/src/libm/s_floor.c"
        "SDL/2.0.4/src/libm/s_scalbn.c"
        "SDL/2.0.4/src/libm/s_sin.c"
        "SDL/2.0.4/src/libm/s_tan.c"
        "SDL/2.0.4/src/SDL.c"
        "SDL/2.0.4/src/SDL_assert.c"
        "SDL/2.0.4/src/atomic/SDL_atomic.c"
        "SDL/2.0.4/src/audio/SDL_audio.c"
        "SDL/2.0.4/src/audio/SDL_audiocvt.c"
        "SDL/2.0.4/src/audio/SDL_audiodev.c"
        "SDL/2.0.4/src/audio/SDL_audiotypecvt.c"
        "SDL/2.0.4/src/render/software/SDL_blendfillrect.c"
        "SDL/2.0.4/src/render/software/SDL_blendline.c"
        "SDL/2.0.4/src/render/software/SDL_blendpoint.c"
        "SDL/2.0.4/src/video/SDL_blit.c"
        "SDL/2.0.4/src/video/SDL_blit_0.c"
        "SDL/2.0.4/src/video/SDL_blit_1.c"
        "SDL/2.0.4/src/video/SDL_blit_A.c"
        "SDL/2.0.4/src/video/SDL_blit_auto.c"
        "SDL/2.0.4/src/video/SDL_blit_copy.c"
        "SDL/2.0.4/src/video/SDL_blit_N.c"
        "SDL/2.0.4/src/video/SDL_blit_slow.c"
        "SDL/2.0.4/src/video/SDL_bmp.c"
        "SDL/2.0.4/src/video/SDL_clipboard.c"
        "SDL/2.0.4/src/events/SDL_clipboardevents.c"
        "SDL/2.0.4/src/cpuinfo/SDL_cpuinfo.c"
        "SDL/2.0.4/src/render/SDL_d3dmath.c"
        "SDL/2.0.4/src/haptic/windows/SDL_dinputhaptic.c"
        "SDL/2.0.4/src/joystick/windows/SDL_dinputjoystick.c"
        "SDL/2.0.4/src/audio/directsound/SDL_directsound.c"
        "SDL/2.0.4/src/audio/disk/SDL_diskaudio.c"
        "SDL/2.0.4/src/render/software/SDL_drawline.c"
        "SDL/2.0.4/src/render/software/SDL_drawpoint.c"
        "SDL/2.0.4/src/events/SDL_dropevents.c"
        "SDL/2.0.4/src/audio/dummy/SDL_dummyaudio.c"
        "SDL/2.0.4/src/dynapi/SDL_dynapi.c"
        "SDL/2.0.4/src/video/SDL_egl.c"
        "SDL/2.0.4/src/SDL_error.c"
        "SDL/2.0.4/src/events/SDL_events.c"
        "SDL/2.0.4/src/video/SDL_fillrect.c"
        "SDL/2.0.4/src/events/SDL_gesture.c"
        "SDL/2.0.4/src/stdlib/SDL_getenv.c"
        "SDL/2.0.4/src/haptic/SDL_haptic.c"
        "SDL/2.0.4/src/SDL_hints.c"
        "SDL/2.0.4/src/stdlib/SDL_iconv.c"
        "SDL/2.0.4/src/events/SDL_keyboard.c"
        "SDL/2.0.4/src/SDL_log.c"
        "SDL/2.0.4/src/stdlib/SDL_malloc.c"
        "SDL/2.0.4/src/audio/SDL_mixer.c"
        "SDL/2.0.4/src/joystick/windows/SDL_mmjoystick.c"
        "SDL/2.0.4/src/events/SDL_mouse.c"
        "SDL/2.0.4/src/video/dummy/SDL_nullevents.c"
        "SDL/2.0.4/src/video/dummy/SDL_nullframebuffer.c"
        "SDL/2.0.4/src/video/dummy/SDL_nullvideo.c"
        "SDL/2.0.4/src/video/SDL_pixels.c"
        "SDL/2.0.4/src/power/SDL_power.c"
        "SDL/2.0.4/src/stdlib/SDL_qsort.c"
        "SDL/2.0.4/src/events/SDL_quit.c"
        "SDL/2.0.4/src/video/SDL_rect.c"
        "SDL/2.0.4/src/render/SDL_render.c"
        "SDL/2.0.4/src/render/direct3d/SDL_render_d3d.c"
        "SDL/2.0.4/src/render/direct3d11/SDL_render_d3d11.c"
        "SDL/2.0.4/src/render/opengl/SDL_render_gl.c"
        "SDL/2.0.4/src/render/opengles2/SDL_render_gles2.c"
        "SDL/2.0.4/src/render/software/SDL_render_sw.c"
        "SDL/2.0.4/src/video/SDL_RLEaccel.c"
        "SDL/2.0.4/src/render/software/SDL_rotate.c"
        "SDL/2.0.4/src/file/SDL_rwops.c"
        "SDL/2.0.4/src/render/opengl/SDL_shaders_gl.c"
        "SDL/2.0.4/src/render/opengles2/SDL_shaders_gles2.c"
        "SDL/2.0.4/src/video/SDL_shape.c"
        "SDL/2.0.4/src/atomic/SDL_spinlock.c"
        "SDL/2.0.4/src/stdlib/SDL_stdlib.c"
        "SDL/2.0.4/src/video/SDL_stretch.c"
        "SDL/2.0.4/src/stdlib/SDL_string.c"
        "SDL/2.0.4/src/video/SDL_surface.c"
        "SDL/2.0.4/src/thread/generic/SDL_syscond.c"
        "SDL/2.0.4/src/filesystem/windows/SDL_sysfilesystem.c"
        "SDL/2.0.4/src/loadso/windows/SDL_sysloadso.c"
        "SDL/2.0.4/src/thread/windows/SDL_sysmutex.c"
        "SDL/2.0.4/src/power/windows/SDL_syspower.c"
        "SDL/2.0.4/src/thread/windows/SDL_syssem.c"
        "SDL/2.0.4/src/thread/windows/SDL_systhread.c"
        "SDL/2.0.4/src/timer/windows/SDL_systimer.c"
        "SDL/2.0.4/src/thread/windows/SDL_systls.c"
        "SDL/2.0.4/src/thread/SDL_thread.c"
        "SDL/2.0.4/src/timer/SDL_timer.c"
        "SDL/2.0.4/src/events/SDL_touch.c"
        "SDL/2.0.4/src/video/SDL_video.c"
        "SDL/2.0.4/src/audio/SDL_wave.c"
        "SDL/2.0.4/src/events/SDL_windowevents.c"
        "SDL/2.0.4/src/core/windows/SDL_windows.c"
        "SDL/2.0.4/src/video/windows/SDL_windowsclipboard.c"
        "SDL/2.0.4/src/video/windows/SDL_windowsevents.c"
        "SDL/2.0.4/src/video/windows/SDL_windowsframebuffer.c"
        "SDL/2.0.4/src/video/windows/SDL_windowskeyboard.c"
        "SDL/2.0.4/src/video/windows/SDL_windowsmessagebox.c"
        "SDL/2.0.4/src/video/windows/SDL_windowsmodes.c"
        "SDL/2.0.4/src/video/windows/SDL_windowsmouse.c"
        "SDL/2.0.4/src/video/windows/SDL_windowsopengl.c"
        "SDL/2.0.4/src/video/windows/SDL_windowsopengles.c"
        "SDL/2.0.4/src/video/windows/SDL_windowsshape.c"
        "SDL/2.0.4/src/video/windows/SDL_windowsvideo.c"
        "SDL/2.0.4/src/video/windows/SDL_windowswindow.c"
        "SDL/2.0.4/src/audio/winmm/SDL_winmm.c"
        "SDL/2.0.4/src/audio/xaudio2/SDL_xaudio2.c"
        "SDL/2.0.4/src/core/windows/SDL_xinput.c"
        "SDL/2.0.4/src/render/SDL_yuv_mmx.c"
        "SDL/2.0.4/src/render/SDL_yuv_sw.c"
    )
elseif(APPLE)
    file(GLOB DEMOS_VENDOR_SDL_SRC
        "SDL/2.0.4/src/*.c"
        "SDL/2.0.4/src/atomic/*.c"
        "SDL/2.0.4/src/audio/*.c"
        "SDL/2.0.4/src/audio/disk/*.c"
        "SDL/2.0.4/src/audio/dummy/*.c"
        "SDL/2.0.4/src/audio/coreaudio/*.c"
        "SDL/2.0.4/src/cpuinfo/*.c"
        "SDL/2.0.4/src/dynapi/*.c"
        "SDL/2.0.4/src/events/*.c"
        "SDL/2.0.4/src/filesystem/cocoa/*.m"
        "SDL/2.0.4/src/file/*.c"
        "SDL/2.0.4/src/file/cocoa/*.m"
        "SDL/2.0.4/src/haptic/*.c"
        "SDL/2.0.4/src/haptic/darwin/*.c"
        "SDL/2.0.4/src/joystick/*.c"
        "SDL/2.0.4/src/joystick/darwin/*.c"
        "SDL/2.0.4/src/loadso/dlopen/*.c"
        "SDL/2.0.4/src/power/*.c"
        "SDL/2.0.4/src/power/macosx/*.c"
        "SDL/2.0.4/src/render/*.c"
        "SDL/2.0.4/src/render/opengl/*.c"
        "SDL/2.0.4/src/render/software/*.c"
        "SDL/2.0.4/src/stdlib/*.c"
        "SDL/2.0.4/src/thread/*.c"
        "SDL/2.0.4/src/thread/pthread/*.c"
        "SDL/2.0.4/src/timer/*.c"
        "SDL/2.0.4/src/timer/unix/*.c"
        "SDL/2.0.4/src/video/*.c"
        "SDL/2.0.4/src/video/cocoa/*.m"
        "SDL/2.0.4/src/video/dummy/*.c"
        "SDL/2.0.4/src/video/x11/*.c"
    )
endif()

add_library(Vendor STATIC ${DEMOS_VENDOR_GLEW_SRC} ${DEMOS_VENDOR_SDL_SRC})

source_group("glew" FILES ${DEMOS_VENDOR_GLEW_SRC})
source_group("SDL" FILES ${DEMOS_VENDOR_SDL_SRC})

target_include_directories(Vendor PRIVATE
    "."
    "glew/1.13/include"
    "SDL/2.0.4/include"
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
