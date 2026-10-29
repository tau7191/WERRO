#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>

class Application {
public:
    Application();
    ~Application();
    void Run();

private:
    void InitWindow();
    void InitVulkan();
    void MainLoop();
    void Cleanup();

    GLFWwindow* m_window = nullptr;
    VkInstance m_vulkanInstance = VK_NULL_HANDLE;

    const uint32_t WIDTH = 1280;
    const uint32_t HEIGHT = 720;
};