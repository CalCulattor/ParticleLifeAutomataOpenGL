#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "ParticleSystem.h"

#include <iostream>
#include <vector>
#include <cmath>

const char* vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec2 position;

void main()
{
    vec2 normalized = position / vec2(800.0, 800.0);
    vec2 clipSpace = normalized * 2.0 - 1.0;

    gl_Position = vec4(clipSpace, 0.0, 1.0);

    gl_PointSize = 5.0;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core

out vec4 FragColor;

void main()
{
    vec2 coord = gl_PointCoord - vec2(0.5);

    float distance = length(coord);
    float edge = fwidth(distance);

    float alpha = 1.0 - smoothstep(0.5 - edge, 0.5, distance);

    FragColor = vec4(1.0, 1.0, 1.0, alpha);
}
)";

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    return shader;
}

int main() {

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Particle Simulator", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGL((GLADloadfunc)glfwGetProcAddress);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ParticleSystem pSystem;

    constexpr int PARTICLE_COUNT = 30;

    for (int i = 0; i < PARTICLE_COUNT; ++i)
    {
        pSystem.spawn(400 + i, 200 + 2*i);
    }
    pSystem.postSpawnCleanup();

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, PARTICLE_COUNT * sizeof(Particle), pSystem.m_Particles.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer (
        0,                  // attribute location
        2,                  // x and y
        GL_FLOAT,           // data type
        GL_FALSE,           // don't normalize
        sizeof(Particle),   // distance between particles
        (void*)0            // position of x
    );

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        pSystem.update();

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, PARTICLE_COUNT * sizeof(Particle), pSystem.m_Particles.data());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, PARTICLE_COUNT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
}