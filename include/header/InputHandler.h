#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <GLFW/glfw3.h>

#include "Simulation.h"

class InputHandler
{
public:
    static void processInput(GLFWwindow* window, Simulation& simulation);

private:
    static bool resetKeyWasPressed;
};

#endif
