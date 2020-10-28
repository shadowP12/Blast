#include <iostream>
#include <GLFW/glfw3.h>
#include <Gfx/GfxDevice.h>
#include <Gfx/Vulkan/VulkanDevice.h>

int main() {
    Blast::VulkanDevice* device = new Blast::VulkanDevice;
    delete device;
//    glfwInit();
//    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
//    GLFWwindow* window = glfwCreateWindow(800, 600, "Blast", nullptr, nullptr);
//    while (!glfwWindowShouldClose(window)) {
//        glfwPollEvents();
//    }
//    glfwDestroyWindow(window);
//    glfwTerminate();
    return 0;
}