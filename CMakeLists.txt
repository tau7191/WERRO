cmake_minimum_required(VERSION 3.20)
project(WERRO VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Статическая линковка рантайма для Windows (чтобы не требовались DLL)
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Проверка наличия Vulkan
find_package(Vulkan REQUIRED)

# Поиск glslc (компилятор шейдеров)
find_program(GLSL_COMPILER NAMES glslc glslangValidator)
if (NOT GLSL_COMPILER)
    if (WIN32)
        set(HINT "Скачайте Vulkan SDK с https://vulkan.lunarg.com/")
    elseif (APPLE)
        set(HINT "Выполните: brew install vulkan-sdk glslang")
    else()
        set(HINT "Выполните: sudo apt install glslc glslang-tools")
    endif()
    message(FATAL_ERROR "ОШИБКА: Не найден glslc.\n${HINT}")
endif()

# Системные библиотеки (GLFW, GLM) - если нет, скачаем
find_package(glfw3 3.3 CONFIG QUIET)
if (NOT glfw3_FOUND)
    include(FetchContent)
    FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.3.9)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)
endif()

find_package(glm CONFIG QUIET)
if (NOT glm_FOUND)
    include(FetchContent)
    FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 0.9.9.8)
    FetchContent_MakeAvailable(glm)
endif()

# Чистые C++ библиотеки
include(FetchContent)
FetchContent_Declare(volk GIT_REPOSITORY https://github.com/zeux/volk.git GIT_TAG master)
FetchContent_MakeAvailable(volk)

FetchContent_Declare(stb GIT_REPOSITORY https://github.com/nothings/stb.git GIT_TAG master)
FetchContent_MakeAvailable(stb)

#FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG docking)
#FetchContent_MakeAvailable(imgui)

# Сборка проекта
set(SOURCES
    src/main.cpp
    src/Application.cpp
    #src/Game.cpp
)

add_executable(werro ${SOURCES})

target_include_directories(werro PRIVATE
    ${Vulkan_INCLUDE_DIRS}
    ${stb_SOURCE_DIR}
    #${imgui_SOURCE_DIR}
    #${imgui_SOURCE_DIR}/backends
    src
)

target_link_libraries(werro PRIVATE
    Vulkan::Vulkan
    glfw
    glm::glm
    volk_headers
)

# ImGui Sources
#set(IMGUI_SOURCES
#    ${imgui_SOURCE_DIR}/imgui.cpp
#    ${imgui_SOURCE_DIR}/imgui_draw.cpp
#    ${imgui_SOURCE_DIR}/imgui_tables.cpp
#    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
#    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
#    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
#)

target_sources(werro PRIVATE ${IMGUI_SOURCES})
target_compile_definitions(werro PRIVATE IMGUI_IMPL_VULKAN IMGUI_IMPL_GLFW)

# Платформенные флаги
if (WIN32)
    target_compile_definitions(werro PRIVATE PLATFORM_WINDOWS)
    target_link_libraries(werro PRIVATE ws2_32)
elseif (APPLE)
    target_compile_definitions(werro PRIVATE PLATFORM_MACOS)
    target_link_libraries(werro PRIVATE "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
else()
    target_compile_definitions(werro PRIVATE PLATFORM_LINUX)
    target_link_libraries(werro PRIVATE pthread dl)
endif()

# Автокомпиляция шейдеров
foreach(SHADER ${SHADERS})
    get_filename_component(FILENAME ${SHADER} NAME)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.spv
        COMMAND ${GLSL_COMPILER} -c ${SHADER} -o ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.spv
        DEPENDS ${SHADER}
        COMMENT "Компиляция шейдера ${FILENAME}"
    )
    list(APPEND SPV_FILES ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.spv)
endforeach()
add_custom_target(shaders ALL DEPENDS ${SPV_FILES})
