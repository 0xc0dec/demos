file(GLOB DEMOS_COMMON "*.cpp" "*.h")
source_group("" FILES ${DEMOS_COMMON})
add_library(Common ${DEMOS_COMMON})

set_default_includes(Common)
set_default_definitions(Common)

if (MSVC)
    target_compile_options(Common PRIVATE /wd4267 /wd4244 /wd4312)
endif()
