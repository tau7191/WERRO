cmake_minimum_required(VERSION 3.20)
project(WERRO
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Vulkan REQUIRED)

find_package(glfw3 3.3 CONFIG)
if (NOT glfw3_FOUND)
    find_package(glfw3 3.3 REQUIRED)
endif()

find_package(glm CONFIG)
if (NOT glm_FOUND)
    find_package(glm REQUIRED)
endif()

include(FetchContent)

FetchContent_Declare(
    volk
    GIT_REPOSITORY https://github.com/zeux/volk.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(volk)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking
)
FetchContent_MakeAvailable(imgui)

set(SOURCES
    src/main.cpp
)

add_executable(werro ${SOURCES})

target_include_directories(werro PRIVATE
    ${Vulkan_INCLUDE_DIRS}
    ${glm_INCLUDE_DIRS}
    ${stb_SOURCE_DIR}
    ${imgui_SOURCE_DIR}
    src
)

target_link_libraries(werro PRIVATE
    Vulkan::Vulkan
    glfw
    glm::glm
    volk_headers
)

set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
)

target_sources(werro PRIVATE ${IMGUI_SOURCES})
target_compile_definitions(werro PRIVATE
    IMGUI_IMPL_VULKAN
    IMGUI_IMPL_GLFW
)

message(STATUS "Vulkan include: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan libraries: ${Vulkan_LIBRARIES}")
message(STATUS "GLFW found: ${glfw3_FOUND}")
message(STATUS "GLM found: ${glm_FOUND}")


if (WIN32)
    target_link_libraries(werro PRIVATE ws2_32)
    target_compile_definitions(werro PRIVATE PLATFORM_WINDOWS)
elseif (APPLE)
    target_compile_definitions(werro PRIVATE PLATFORM_MACOS)
else()
    target_compile_definitions(werro PRIVATE PLATFORM_LINUX)
endif()

