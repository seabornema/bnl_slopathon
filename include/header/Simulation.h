#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>

#include "Circle.h"
#include "Config.h"

class Simulation
{
public:
    Simulation();

    void initialize();
    void update(float deltaTime);

    const std::vector<Circle>& getCircles() const;

    void reset();

private:
    std::vector<Circle> circles;

    void generateRubble();
    void integrate(float deltaTime);
    void applyFriction(float deltaTime);
    void resolveCollisions();
    void resolveBoundaryCollisions();

    float randomFloat(float min, float max) const;
};

#endif
