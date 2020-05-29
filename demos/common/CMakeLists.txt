file(GLOB SRC "*.cpp" "*.h")
file(GLOB GL_SRC "gl/*.cpp" "gl/*.h")
file(GLOB VK_SRC "vk/*.cpp" "vk/*.h")
source_group("" FILES ${SRC})
source_group("gl" FILES ${GL_SRC})
source_group("vk" FILES ${VK_SRC})
add_library(Common ${SRC} ${GL_SRC} ${VK_SRC})

set_default_includes(Common)
set_default_definitions(Common)

if (MSVC)
    target_compile_options(Common PRIVATE /wd4267 /wd4244 /wd4312)
endif()
