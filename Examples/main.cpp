#include <iostream>
#include <GLFW/glfw3.h>
#include <Gfx/GfxContext.h>
#include <Gfx/Vulkan/VulkanContext.h>

int main() {
    Blast::VulkanContext* context = new Blast::VulkanContext;
    delete context;
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