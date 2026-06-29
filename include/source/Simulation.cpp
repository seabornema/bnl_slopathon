#include "Simulation.h"

#include <cmath>
#include <cstdlib>
#include <algorithm>

Simulation::Simulation()
{
}

void Simulation::initialize()
{
    generateRubble();
}

void Simulation::update(float deltaTime)
{
    if (deltaTime <= 0.0f)
    {
        return;
    }

    // Clamp large frame spikes so the simulation does not explode
    deltaTime = std::min(deltaTime, Config::MAX_DELTA_TIME);

    applyFriction(deltaTime);
    integrate(deltaTime);
    resolveBoundaryCollisions();
    resolveCollisions();
}

const std::vector<Circle>& Simulation::getCircles() const
{
    return circles;
}

void Simulation::reset()
{
    circles.clear();
    generateRubble();
}

void Simulation::generateRubble()
{
    circles.clear();
    circles.reserve(Config::RUBBLE_COUNT);

    int attempts = 0;
    const int maxAttempts = Config::RUBBLE_COUNT * 100;

    while (static_cast<int>(circles.size()) < Config::RUBBLE_COUNT &&
           attempts < maxAttempts)
    {
        attempts++;

        float radius = randomFloat(Config::MIN_RUBBLE_RADIUS,
                                   Config::MAX_RUBBLE_RADIUS);

        float x = randomFloat(Config::SIMULATION_LEFT + radius,
                              Config::SIMULATION_RIGHT - radius);

        float y = randomFloat(Config::SIMULATION_BOTTOM + radius,
                              Config::SIMULATION_TOP - radius);

        Circle candidate;
        candidate.position = Vec2(x, y);
        candidate.velocity = Vec2(
            randomFloat(-Config::INITIAL_SPEED, Config::INITIAL_SPEED),
            randomFloat(-Config::INITIAL_SPEED, Config::INITIAL_SPEED)
        );
        candidate.radius = radius;

        // Heavier rubble pieces move less when collisions are resolved.
        // Area is a simple mass approximation.
        candidate.mass = radius * radius;

        bool overlaps = false;

        for (const Circle& other : circles)
        {
            Vec2 difference = candidate.position - other.position;
            float distanceSquared = difference.lengthSquared();
            float minDistance = candidate.radius + other.radius;

            if (distanceSquared < minDistance * minDistance)
            {
                overlaps = true;
                break;
            }
        }

        if (!overlaps)
        {
            circles.push_back(candidate);
        }
    }
}

void Simulation::integrate(float deltaTime)
{
    for (Circle& circle : circles)
    {
        circle.position += circle.velocity * deltaTime;
    }
}

void Simulation::applyFriction(float deltaTime)
{
    for (Circle& circle : circles)
    {
        float speed = circle.velocity.length();

        if (speed <= Config::STOP_SPEED)
        {
            circle.velocity = Vec2(0.0f, 0.0f);
            continue;
        }

        Vec2 direction = circle.velocity / speed;

        float frictionAmount = Config::FRICTION_COEFFICIENT * deltaTime;
        float newSpeed = std::max(0.0f, speed - frictionAmount);

        circle.velocity = direction * newSpeed;
    }
}

void Simulation::resolveCollisions()
{
    for (std::size_t i = 0; i < circles.size(); i++)
    {
        for (std::size_t j = i + 1; j < circles.size(); j++)
        {
            Circle& a = circles[i];
            Circle& b = circles[j];

            Vec2 difference = b.position - a.position;
            float distanceSquared = difference.lengthSquared();

            float minDistance = a.radius + b.radius;

            if (distanceSquared >= minDistance * minDistance)
            {
                continue;
            }

            float distance = std::sqrt(distanceSquared);

            Vec2 normal;

            if (distance > 0.0001f)
            {
                normal = difference / distance;
            }
            else
            {
                normal = Vec2(1.0f, 0.0f);
                distance = minDistance;
            }

            float penetration = minDistance - distance;

            float totalMass = a.mass + b.mass;

            if (totalMass <= 0.0f)
            {
                continue;
            }

            // Positional correction so circles are no longer overlapping
            a.position -= normal * (penetration * (b.mass / totalMass));
            b.position += normal * (penetration * (a.mass / totalMass));

            // Relative velocity along the collision normal
            Vec2 relativeVelocity = b.velocity - a.velocity;
            float velocityAlongNormal = relativeVelocity.dot(normal);

            // If circles are already separating, do not apply impulse
            if (velocityAlongNormal > 0.0f)
            {
                continue;
            }

            float restitution = Config::COLLISION_RESTITUTION;

            float inverseMassA = 1.0f / a.mass;
            float inverseMassB = 1.0f / b.mass;

            float impulseMagnitude =
                -(1.0f + restitution) * velocityAlongNormal /
                (inverseMassA + inverseMassB);

            Vec2 impulse = normal * impulseMagnitude;

            a.velocity -= impulse * inverseMassA;
            b.velocity += impulse * inverseMassB;
        }
    }
}

void Simulation::resolveBoundaryCollisions()
{
    for (Circle& circle : circles)
    {
        if (circle.position.x - circle.radius < Config::SIMULATION_LEFT)
        {
            circle.position.x = Config::SIMULATION_LEFT + circle.radius;
            circle.velocity.x *= -Config::WALL_RESTITUTION;
        }

        if (circle.position.x + circle.radius > Config::SIMULATION_RIGHT)
        {
            circle.position.x = Config::SIMULATION_RIGHT - circle.radius;
            circle.velocity.x *= -Config::WALL_RESTITUTION;
        }

        if (circle.position.y - circle.radius < Config::SIMULATION_BOTTOM)
        {
            circle.position.y = Config::SIMULATION_BOTTOM + circle.radius;
            circle.velocity.y *= -Config::WALL_RESTITUTION;
        }

        if (circle.position.y + circle.radius > Config::SIMULATION_TOP)
        {
            circle.position.y = Config::SIMULATION_TOP - circle.radius;
            circle.velocity.y *= -Config::WALL_RESTITUTION;
        }
    }
}

float Simulation::randomFloat(float min, float max) const
{
    float t = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    return min + t * (max - min);
}
