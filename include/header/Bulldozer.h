#ifndef BULLDOZER_H
#define BULLDOZER_H

#include "Circle.h"

class Bulldozer
{
public:
    Bulldozer();

    void reset();
    void update(float rotationCommand, float movementCommand, float deltaTime);

    Vec2 getForwardVector() const;
    Vec2 getRightVector() const;

    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;

    float rotationRadians;
};

#endif
