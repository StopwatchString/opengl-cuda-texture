#include <iostream>

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glh/glh.h"
#include "GLFW/glfw3.h"

#include <cuda_runtime.h>
#include <device_launch_parameters.h>

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

typedef struct Vertex
{
    glm::vec2 pos;
    glm::vec3 col;
} Vertex;

Vertex vertices[3] =
{
    { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } },
    { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } },
    { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } }
};

extern "C" void launchHelloKernel(char* str, int size);

int main()
{
    const char* original = "Hello World from CPU!";
    char* str = new char[strlen(original) + 1];
    strcpy(str, original);

    std::cout << "Original string: " << str << std::endl;

    launchHelloKernel(str, strlen(str) + 1);

    std::cout << "Modified string: " << str << std::endl;

    delete[] str;
   
    const char* vertex_shader_text = glh::utils::loadFile("C:\\dev\\cuda-texture\\res\\default.vert");
    const char* fragment_shader_text = glh::utils::loadFile("C:\\dev\\cuda-texture\\res\\default.frag");

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); // COMPAT PROFILE CHOSEN

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Producer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(1);

    // Initialize OpenGL Resources
    GLuint vertex_buffer;
    glh::VBO::create(vertex_buffer);
    glh::VBO::bind(vertex_buffer);
    glh::VBO::allocateBuffer(sizeof(vertices), vertices, GL_DYNAMIC_DRAW, vertex_buffer);

    const GLuint vertex_shader = glh::shader::create(GL_VERTEX_SHADER);
    glh::shader::attachSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glh::shader::compileShader(vertex_shader);

    const GLuint fragment_shader = glh::shader::create(GL_FRAGMENT_SHADER);
    glh::shader::attachSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glh::shader::compileShader(fragment_shader);

    const GLuint program = glh::program::create();
    glh::program::attachShader(program, vertex_shader);
    glh::program::attachShader(program, fragment_shader);
    glh::program::linkProgram(program);

    const GLint mvp_location = glh::program::getUniformLocation(program, "MVP");
    const GLint vpos_location = glh::program::getAttribLocation(program, "vPos");
    const GLint vcol_location = glh::program::getAttribLocation(program, "vCol");

    GLuint vertex_array;
    glh::VAO::create(vertex_array);
    glh::VAO::bind(vertex_array);
    glh::VAO::enableVertexAttribArray(vpos_location);
    glh::VAO::enableVertexAttribArray(vpos_location);
    glh::VAO::vertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glh::VAO::enableVertexAttribArray(vcol_location);
    glh::VAO::vertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (void*)offsetof(Vertex, col));


    while (!glfwWindowShouldClose(window)) {

        glClearColor(0.0, 0.5, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glh::program::bind(program);

        glm::mat4 mvp(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&mvp);

        glh::VBO::updateBuffer(0, sizeof(vertices), vertices);

        glh::VAO::bind(vertex_array);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();

        glh::utils::glErrorCheck("End of render loop");
    }

    glfwTerminate();
    return 0;
}