#include <stdio.h>
#include <glad.h>
#include <glfw.h>

int main() {
    if (!glfwInit()) {
        puts("Failed to initialize glfw");
        return 1;
    }
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow* context = glfwCreateWindow(1,1,"offscreen", NULL, NULL);
    if (!context) {
        puts("Failed to make context");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(context);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        puts("Failed to make context current");
        glfwDestroyWindow(context);
        glfwTerminate();
        return 1;
    }

    puts("Loaded successfully");

    glfwDestroyWindow(context);
    glfwTerminate();
    return 0;
}