
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include <yaml-cpp/yaml.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "mapper/core/Mapper.h"

#include "OpenGL_util/core/Renderer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "config.h"

#include "imgui_helper/imgui.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // Window hints for glfw
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(conf.WIN_WIDTH, conf.WIN_HEIGHT, "UV-Mapper", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        std::cout << "Could not init glew." << std::endl;
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Set Blending
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_BLEND));

    Renderer renderer;

    ImGuiIO& io = ImGuiInit();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    SetupImGuiStyle();

    // Game
    Mapper UvMapper = Mapper();
        
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();
        GLCall(glClearColor(0.f, 0.f, 0.f, 1.f));

        ImGuiNewFrame();

        /*ImGui::Begin("Test Window");
        ImGui::Text("Press this Button :)");
        if (ImGui::Button("Button")) LOGC("Button Press!", LOG_COLOR::SPECIAL_A);
        ImGui::End();*/

        //Game
        UvMapper.OnInput(window);
        UvMapper.OnUpdate();
        UvMapper.OnRender();
        UvMapper.OnGuiRender();

        ImGuiRender(io);

        // glfw handling
        GLCall(glfwSwapBuffers(window));
        GLCall(glfwPollEvents());
    }

    LOGC("Terminating Application...", LOG_COLOR::LOG);
    ImGuiShutdown();
    glfwTerminate();

    LOGC("OK. Exiting", LOG_COLOR::OK);
    return 0;
}