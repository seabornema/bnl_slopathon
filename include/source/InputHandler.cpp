#include "InputHandler.h"

bool InputHandler::resetKeyWasPressed = false;

void InputHandler::processInput(GLFWwindow* window, Simulation& simulation)
{
    if (window == nullptr)
    {
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    bool resetKeyIsPressed =
        glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;

    if (resetKeyIsPressed && !resetKeyWasPressed)
    {
        simulation.reset();
    }

    resetKeyWasPressed = resetKeyIsPressed;
}
