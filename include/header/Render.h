#ifndef RENDER_H
#define RENDER_H

#include <vector>

#include <glad/glad.h>

#include "Bulldozer.h"
#include "Circle.h"
#include "Simulation.h"

class Render
{
public:
    Render();
    ~Render();

    Render(const Render&) = delete;
    Render& operator=(const Render&) = delete;

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
    void drawCircleOutline(const Circle& circle);

    void drawBulldozer(const Bulldozer& bulldozer);
    void drawBulldozerBody(const Bulldozer& bulldozer);
    void drawBulldozerBlade(const Bulldozer& bulldozer);

    void uploadAndDraw(const std::vector<float>& vertices, GLenum drawMode);

    void setColor(float r, float g, float b, float a);
};

#endif
