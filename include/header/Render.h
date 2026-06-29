#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#include <glad/glad.h>

#include "Simulation.h"
#include "Circle.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void render(const Simulation& simulation);

private:
    GLuint shaderProgram;
    GLuint vao;
    GLuint vbo;

    GLint colorUniformLocation;

    void initializeOpenGLObjects();
    void destroyOpenGLObjects();

    GLuint createShader(GLenum shaderType, const char* source);
    GLuint createShaderProgram();

    void drawRoof();
    void drawBoundary();
    void drawCircle(const Circle& circle);

    void uploadAndDraw(const std::vector<float>& vertices, GLenum drawMode);

    void setColor(float r, float g, float b, float a);
};

#endif
