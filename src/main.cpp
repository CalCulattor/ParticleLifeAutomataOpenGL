#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "ParticleSystem.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <cstddef>

const char* vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in int particleId;

flat out int id;

void main()
{
    vec2 normalized = position / vec2(800.0, 800.0);
    vec2 clipSpace = normalized * 2.0 - 1.0;

    gl_Position = vec4(clipSpace, 0.0, 1.0);
    gl_PointSize = 5.0;

    id = int(particleId);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core

flat in int id;

out vec4 FragColor;

void main()
{
    vec2 coord = gl_PointCoord - vec2(0.5);
    float distance = length(coord);

    float edge = fwidth(distance);
    float alpha = 1.0 - smoothstep(0.5 - edge, 0.5, distance);

    vec3 color;

    if (id == 0)
        color = vec3(1.0, 0.0, 0.0); // red
    else if (id == 1)
        color = vec3(0.0, 1.0, 0.0); // green
    else if (id == 2)
        color = vec3(0.0, 0.0, 1.0); // blue
    else
        color = vec3(1.0);             // white

    FragColor = vec4(color, alpha);
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

    constexpr int PARTICLE_COUNT = 10000;

    constexpr int SIDE = 100;
    constexpr float START = 25.0f;
    constexpr float SPACING = 7.5f;

    for (int y = 0; y < SIDE; ++y) {
        for (int x = 0; x < SIDE; ++x) {

            float px = START + x * SPACING;
            float py = START + y * SPACING;

            pSystem.spawn(px, py, (x + y) % 2 + 1);
        }
    }

    pSystem.addInteraction(1, 2, -0.1);
    pSystem.addInteraction(2, 1, 2);
    pSystem.addInteraction(1, 1, 0.7);
    pSystem.addInteraction(2, 2, 0);

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

    glVertexAttribIPointer(
        1,
        1,
        GL_INT,
        sizeof(Particle),
        (void*)offsetof(Particle, type_Id)
    );

    glEnableVertexAttribArray(1);

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