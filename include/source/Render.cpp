#include "Render.h"

#include <cmath>
#include <iostream>
#include <vector>

#include "Config.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Render::Render()
    : shaderProgram(0),
      vao(0),
      vbo(0),
      colorUniformLocation(-1)
{
    initializeOpenGLObjects();
}

Render::~Render()
{
    destroyOpenGLObjects();
}

void Render::render(const Simulation& simulation)
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

    drawBulldozer(simulation.getBulldozer());

    glBindVertexArray(0);
    glUseProgram(0);
}


void Render::initializeOpenGLObjects()
{
    shaderProgram = createShaderProgram();

    colorUniformLocation = glGetUniformLocation(shaderProgram, "uColor");

    if (colorUniformLocation == -1)
    {
        std::cerr << "Warning: could not find shader uniform uColor" << std::endl;
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

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

void Render::destroyOpenGLObjects()
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

GLuint Render::createShader(GLenum shaderType, const char* source)
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

GLuint Render::createShaderProgram()
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

void Render::drawRoof()
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

void Render::drawBoundary()
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

void Render::drawCircle(const Circle& circle)
{
    std::vector<float> vertices;

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

    if (circle.material == CircleMaterial::GraphiteRodEnd)
    {
        setColor(
            Config::GRAPHITE_RED,
            Config::GRAPHITE_GREEN,
            Config::GRAPHITE_BLUE,
            1.0f
        );
    }
    else
    {
        setColor(
            Config::RUBBLE_RED,
            Config::RUBBLE_GREEN,
            Config::RUBBLE_BLUE,
            1.0f
        );
    }

    uploadAndDraw(vertices, GL_TRIANGLE_FAN);

    drawCircleOutline(circle);
}

void Render::drawCircleOutline(const Circle& circle)
{
    std::vector<float> outlineVertices;

    outlineVertices.reserve((Config::CIRCLE_SEGMENTS + 1) * 2);

    for (int i = 0; i <= Config::CIRCLE_SEGMENTS; i++)
    {
        float angle =
            static_cast<float>(i) /
            static_cast<float>(Config::CIRCLE_SEGMENTS) *
            2.0f *
            static_cast<float>(M_PI);

        float x = circle.position.x + std::cos(angle) * circle.radius;
        float y = circle.position.y + std::sin(angle) * circle.radius;

        outlineVertices.push_back(x);
        outlineVertices.push_back(y);
    }

    if (circle.material == CircleMaterial::GraphiteRodEnd)
    {
        setColor(0.65f, 0.65f, 0.65f, 1.0f);
    }
    else
    {
        setColor(0.25f, 0.25f, 0.25f, 1.0f);
    }

    glLineWidth(1.5f);
    uploadAndDraw(outlineVertices, GL_LINE_STRIP);
}

void Render::uploadAndDraw(const std::vector<float>& vertices, GLenum drawMode)
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

void Render::setColor(float r, float g, float b, float a)
{
    glUniform4f(colorUniformLocation, r, g, b, a);
}


void Render::drawBulldozer(const Bulldozer& bulldozer)
{
    drawBulldozerBody(bulldozer);
    drawBulldozerBlade(bulldozer);
}

void Render::drawBulldozerBody(const Bulldozer& bulldozer)
{
    std::vector<float> vertices;

    vertices.reserve((Config::CIRCLE_SEGMENTS + 2) * 2);

    vertices.push_back(bulldozer.position.x);
    vertices.push_back(bulldozer.position.y);

    for (int i = 0; i <= Config::CIRCLE_SEGMENTS; i++)
    {
        float angle =
            static_cast<float>(i) /
            static_cast<float>(Config::CIRCLE_SEGMENTS) *
            2.0f *
            static_cast<float>(M_PI);

        float x =
            bulldozer.position.x +
            std::cos(angle) * Config::BULLDOZER_RADIUS;

        float y =
            bulldozer.position.y +
            std::sin(angle) * Config::BULLDOZER_RADIUS;

        vertices.push_back(x);
        vertices.push_back(y);
    }

    setColor(
        Config::BULLDOZER_RED,
        Config::BULLDOZER_GREEN,
        Config::BULLDOZER_BLUE,
        1.0f
    );

    uploadAndDraw(vertices, GL_TRIANGLE_FAN);
}

void Render::drawBulldozerBlade(const Bulldozer& bulldozer)
{
    Vec2 forward = bulldozer.getForwardVector();
    Vec2 right = bulldozer.getRightVector();

    Vec2 bladeCenter =
        bulldozer.position +
        forward * Config::BULLDOZER_BLADE_OFFSET;

    Vec2 bladeA =
        bladeCenter -
        right * (Config::BULLDOZER_BLADE_WIDTH * 0.5f);

    Vec2 bladeB =
        bladeCenter +
        right * (Config::BULLDOZER_BLADE_WIDTH * 0.5f);

    std::vector<float> vertices =
    {
        bladeA.x, bladeA.y,
        bladeB.x, bladeB.y
    };

    setColor(
        Config::BULLDOZER_BLADE_RED,
        Config::BULLDOZER_BLADE_GREEN,
        Config::BULLDOZER_BLADE_BLUE,
        1.0f
    );

    glLineWidth(5.0f);
    uploadAndDraw(vertices, GL_LINES);
}
