#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <cmath>

struct Particle {
    float x;
    float y;
};

const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec2 position;
void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    gl_PointSize = 5.0;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
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

    GLFWwindow* window = glfwCreateWindow(800, 600, "Particle Simulator", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGL((GLADloadfunc)glfwGetProcAddress);

    constexpr int PARTICLE_COUNT = 100;
    std::vector<Particle> particles(PARTICLE_COUNT);
    constexpr int GRID_SIZE = 10;

    for (int i = 0; i < PARTICLE_COUNT; ++i) {
        int x = i % GRID_SIZE;
        int y = i / GRID_SIZE;
        particles[i].x = -0.9f + x * (1.8f / (GRID_SIZE - 1));
        particles[i].y = -0.9f + y * (1.8f / (GRID_SIZE - 1));
    }

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
    glBufferData(GL_ARRAY_BUFFER, PARTICLE_COUNT * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW);

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
        // ====================================================
        // PHYSICS
        // ====================================================
        //
        // This is where your Particle Life simulation goes.
        //
        // For example:
        //
        // 1. Find nearby particles using your spatial grid.
        //
        // 2. Calculate forces:
        //
        //        force = interaction(...)
        //
        // 3. Update acceleration.
        //
        // 4. Update velocity.
        //
        // 5. Update position.
        //
        // 6. Apply friction / boundaries.
        //
        // Example:
        //
        // for (Particle& particle : particles)
        // {
        //     calculateForces(particle, particles);
        //     particle.velocity += particle.acceleration * dt;
        //     particle.x += particle.velocity.x * dt;
        //     particle.y += particle.velocity.y * dt;
        // }
        //
        // ====================================================

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferSubData(GL_ARRAY_BUFFER, 0, PARTICLE_COUNT * sizeof(Particle), particles.data());
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