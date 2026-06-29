#include "Bulldozer.h"

#include <algorithm>
#include <cmath>

#include "Config.h"

Bulldozer::Bulldozer()
{
    reset();
}

void Bulldozer::reset()
{
    position = Vec2(Config::BULLDOZER_START_X, Config::BULLDOZER_START_Y);
    velocity = Vec2(0.0f, 0.0f);
    acceleration = Vec2(0.0f, 0.0f);

    // Start facing upward.
    rotationRadians = 1.57079632679f;
}

void Bulldozer::update(
    float rotationCommand,
    float movementCommand,
    float deltaTime
)
{
    rotationCommand = std::clamp(rotationCommand, -1.0f, 1.0f);
    movementCommand = std::clamp(movementCommand, -1.0f, 1.0f);

    rotationRadians +=
        rotationCommand *
        Config::BULLDOZER_ROTATION_SPEED *
        deltaTime;

    Vec2 forward = getForwardVector();

    acceleration =
        forward *
        movementCommand *
        Config::BULLDOZER_ACCELERATION;

    velocity += acceleration * deltaTime;

    float speed = velocity.length();

    if (speed > Config::BULLDOZER_MAX_SPEED)
    {
        velocity = velocity.normalized() * Config::BULLDOZER_MAX_SPEED;
    }

    velocity *= std::max(0.0f, 1.0f - Config::BULLDOZER_FRICTION * deltaTime);

    position += velocity * deltaTime;

    if (position.x - Config::BULLDOZER_RADIUS < Config::SIMULATION_LEFT)
    {
        position.x = Config::SIMULATION_LEFT + Config::BULLDOZER_RADIUS;
        velocity.x = 0.0f;
    }

    if (position.x + Config::BULLDOZER_RADIUS > Config::SIMULATION_RIGHT)
    {
        position.x = Config::SIMULATION_RIGHT - Config::BULLDOZER_RADIUS;
        velocity.x = 0.0f;
    }

    if (position.y - Config::BULLDOZER_RADIUS < Config::SIMULATION_BOTTOM)
    {
        position.y = Config::SIMULATION_BOTTOM + Config::BULLDOZER_RADIUS;
        velocity.y = 0.0f;
    }

    // Let the bulldozer reach the top edge, but not leave the area.
    if (position.y + Config::BULLDOZER_RADIUS > Config::SIMULATION_TOP)
    {
        position.y = Config::SIMULATION_TOP - Config::BULLDOZER_RADIUS;
        velocity.y = 0.0f;
    }
}

Vec2 Bulldozer::getForwardVector() const
{
    return Vec2(std::cos(rotationRadians), std::sin(rotationRadians));
}

Vec2 Bulldozer::getRightVector() const
{
    Vec2 forward = getForwardVector();
    return Vec2(forward.y, -forward.x);
}
