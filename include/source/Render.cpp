#include "Render.h"

#include <cmath>
#include <iostream>

#include "Config.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Renderer::Renderer()
    : shaderProgram(0),
      vao(0),
      vbo(0),
      colorUniformLocation(-1)
{
    initializeOpenGLObjects();
}

Renderer::~Renderer()
{
    destroyOpenGLObjects();
}

void Renderer::render(const Simulation& simulation)
{
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);

    drawRoof();
    drawBoundary();

    const std::vector<Circle>& circles = simulation.getCircles();

    for (const Circle& circle : circles)
    {
        drawCircle(circle);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void Renderer::initializeOpenGLObjects()
{
    shaderProgram = createShaderProgram();

    colorUniformLocation = glGetUniformLocation(shaderProgram, "uColor");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Each vertex has two floats: x, y
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(float),
        reinterpret_cast<void*>(0)
    );

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::destroyOpenGLObjects()
{
    if (vbo != 0)
    {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }

    if (vao != 0)
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    if (shaderProgram != 0)
    {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}

GLuint Renderer::createShader(GLenum shaderType, const char* source)
{
    GLuint shader = glCreateShader(shaderType);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);

        std::cerr << "Shader compilation failed:\n"
                  << infoLog << std::endl;
    }

    return shader;
}

GLuint Renderer::createShaderProgram()
{
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec2 aPosition;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPosition.x, aPosition.y, 0.0, 1.0);\n"
        "}\n";

    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "uniform vec4 uColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor = uColor;\n"
        "}\n";

    GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);

        std::cerr << "Shader program linking failed:\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void Renderer::drawRoof()
{
    std::vector<float> vertices =
    {
        Config::SIMULATION_LEFT,  Config::SIMULATION_BOTTOM,
        Config::SIMULATION_RIGHT, Config::SIMULATION_BOTTOM,
        Config::SIMULATION_RIGHT, Config::SIMULATION_TOP,

        Config::SIMULATION_LEFT,  Config::SIMULATION_BOTTOM,
        Config::SIMULATION_RIGHT, Config::SIMULATION_TOP,
        Config::SIMULATION_LEFT,  Config::SIMULATION_TOP
    };

    setColor(
        Config::ROOF_RED,
        Config::ROOF_GREEN,
        Config::ROOF_BLUE,
        1.0f
    );

    uploadAndDraw(vertices, GL_TRIANGLES);
}

void Renderer::drawBoundary()
{
    std::vector<float> vertices =
    {
        Config::SIMULATION_LEFT,  Config::SIMULATION_BOTTOM,
        Config::SIMULATION_RIGHT, Config::SIMULATION_BOTTOM,

        Config::SIMULATION_RIGHT, Config::SIMULATION_BOTTOM,
        Config::SIMULATION_RIGHT, Config::SIMULATION_TOP,

        Config::SIMULATION_RIGHT, Config::SIMULATION_TOP,
        Config::SIMULATION_LEFT,  Config::SIMULATION_TOP,

        Config::SIMULATION_LEFT,  Config::SIMULATION_TOP,
        Config::SIMULATION_LEFT,  Config::SIMULATION_BOTTOM
    };

    setColor(
        Config::BOUNDARY_RED,
        Config::BOUNDARY_GREEN,
        Config::BOUNDARY_BLUE,
        1.0f
    );

    glLineWidth(2.0f);
    uploadAndDraw(vertices, GL_LINES);
}

void Renderer::drawCircle(const Circle& circle)
{
    std::vector<float> vertices;

    // Triangle fan:
    // first vertex is the center, then points around the circumference.
    vertices.reserve((Config::CIRCLE_SEGMENTS + 2) * 2);

    vertices.push_back(circle.position.x);
    vertices.push_back(circle.position.y);

    for (int i = 0; i <= Config::CIRCLE_SEGMENTS; i++)
    {
        float angle =
            static_cast<float>(i) /
            static_cast<float>(Config::CIRCLE_SEGMENTS) *
            2.0f *
            static_cast<float>(M_PI);

        float x = circle.position.x + std::cos(angle) * circle.radius;
        float y = circle.position.y + std::sin(angle) * circle.radius;

        vertices.push_back(x);
        vertices.push_back(y);
    }

    setColor(
        Config::RUBBLE_RED,
        Config::RUBBLE_GREEN,
        Config::RUBBLE_BLUE,
        1.0f
    );

    uploadAndDraw(vertices, GL_TRIANGLE_FAN);
}

void Renderer::uploadAndDraw(const std::vector<float>& vertices, GLenum drawMode)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
        vertices.data(),
        GL_DYNAMIC_DRAW
    );

    GLsizei vertexCount = static_cast<GLsizei>(vertices.size() / 2);

    glDrawArrays(drawMode, 0, vertexCount);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::setColor(float r, float g, float b, float a)
{
    glUniform4f(colorUniformLocation, r, g, b, a);
}
