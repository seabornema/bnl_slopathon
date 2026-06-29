#include <iostream>
#include <random>
#include "Simulation.h"
#include <limits>

#include <cmath>
#include <cstdlib>
#include <algorithm>

static float clampFloat(float value, float minValue, float maxValue)
{
    return std::max(minValue, std::min(value, maxValue));
}

static Vec2 closestPointOnSegment(
    const Vec2& point,
    const Vec2& segmentA,
    const Vec2& segmentB
)
{
    Vec2 segment = segmentB - segmentA;
    float lengthSquared = segment.lengthSquared();

    if (lengthSquared <= 0.0001f)
    {
        return segmentA;
    }

    float t = (point - segmentA).dot(segment) / lengthSquared;
    t = clampFloat(t, 0.0f, 1.0f);

    return segmentA + segment * t;
}
void Simulation::initialize()
{
  clearedGraphiteCount = 0;
      bulldozer.reset();
    generateRubble();
}
void Simulation::update(float deltaTime)
{
    if (deltaTime <= 0.0f)
    {
        return;
    }

    deltaTime = std::min(deltaTime, Config::MAX_DELTA_TIME);

    updateBulldozer(deltaTime);

    applyFriction(deltaTime);
    integrate(deltaTime);

    resolveBulldozerCollisions(deltaTime);
    resolveBoundaryCollisions();
    resolveCollisions();

    removeClearedGraphite();
}

void Simulation::removeClearedGraphite()
{
    auto newEnd = std::remove_if(
        circles.begin(),
        circles.end(),
        [this](const Circle& circle)
        {
            bool cleared =
                circle.material == CircleMaterial::GraphiteRodEnd &&
                circle.position.y - circle.radius > Config::SIMULATION_TOP;

            if (cleared)
            {
                clearedGraphiteCount++;
            }

            return cleared;
        }
    );

    circles.erase(newEnd, circles.end());
}

void Simulation::resolveBulldozerCollisions(float deltaTime)
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

    float bladeThickness =
        Config::BULLDOZER_BLADE_THICKNESS;

    int bladeContacts = 0;
    float totalContactMass = 0.0f;

    for (Circle& circle : circles)
    {
        Vec2 closestPoint =
            closestPointOnSegment(circle.position, bladeA, bladeB);

        Vec2 difference =
            circle.position - closestPoint;

        float minDistance =
            circle.radius + bladeThickness;

        float distanceSquared =
            difference.lengthSquared();

        if (distanceSquared >= minDistance * minDistance)
        {
            continue;
        }

        bladeContacts++;
        totalContactMass += circle.mass;

        float distance = std::sqrt(distanceSquared);

        Vec2 normal;

        if (distance > 0.0001f)
        {
            normal = difference / distance;
        }
        else
        {
            normal = forward;
            distance = minDistance;
        }

        float penetration = minDistance - distance;

        circle.position += normal * penetration;

        float bulldozerSpeedIntoContact =
            bulldozer.velocity.dot(normal);

        if (bulldozerSpeedIntoContact > 0.0f)
        {
            float pushImpulse =
                bulldozerSpeedIntoContact *
                Config::BULLDOZER_RECOIL_MULTIPLIER;

            circle.velocity += normal * pushImpulse;

            float recoilImpulse =
                pushImpulse *
                (circle.mass / Config::BULLDOZER_MASS);

            bulldozer.velocity -= normal * recoilImpulse;
        }

        circle.velocity += forward * Config::BULLDOZER_BLADE_PUSH;
    }

    if (bladeContacts > 0)
    {
        float forwardSpeed =
            bulldozer.velocity.dot(forward);

        if (forwardSpeed > 0.0f)
        {
            float loadDrag =
                Config::BULLDOZER_LOAD_DRAG +
                Config::BULLDOZER_LOAD_DRAG_PER_CONTACT *
                static_cast<float>(bladeContacts);

            float massRatio =
                totalContactMass /
                std::max(Config::BULLDOZER_MASS, 0.0001f);

            float slowdown =
                forwardSpeed *
                loadDrag *
                massRatio *
                deltaTime;
            slowdown = std::min(slowdown, forwardSpeed);

            bulldozer.velocity -= forward * slowdown;
        }
    }
}
void Simulation::updateBulldozer(float deltaTime)
{
    float rotationCommand = 0.0f;
    float movementCommand = 0.0f;

    if (neuralNetwork.isLoaded())
    {
        std::vector<float> inputs = buildNetworkInputs();
        std::vector<float> outputs = neuralNetwork.forward(inputs);

        if (outputs.size() >= 2)
        {
            rotationCommand = outputs[0];
            movementCommand = outputs[1];
        }
    }

    bulldozer.update(rotationCommand, movementCommand, deltaTime);
}
std::vector<float> Simulation::buildNetworkInputs() const
{
    std::vector<float> inputs;

    std::vector<float> sightlines = castSightlines();

    for (float value : sightlines)
    {
        inputs.push_back(value);
    }

    Vec2 forward = bulldozer.getForwardVector();

    inputs.push_back(forward.x);
    inputs.push_back(forward.y);

    inputs.push_back(bulldozer.velocity.x / Config::BULLDOZER_MAX_SPEED);
    inputs.push_back(bulldozer.velocity.y / Config::BULLDOZER_MAX_SPEED);

    inputs.push_back(
        bulldozer.acceleration.x / Config::BULLDOZER_ACCELERATION
    );

    inputs.push_back(
        bulldozer.acceleration.y / Config::BULLDOZER_ACCELERATION
    );

    return inputs;
}

std::vector<float> Simulation::castSightlines() const
{
    std::vector<float> sightlines;
    sightlines.reserve(Config::SIGHTLINE_COUNT);

    float startAngle =
        bulldozer.rotationRadians -
        Config::SIGHTLINE_FOV_RADIANS * 0.5f;

    float angleStep = 0.0f;

    if (Config::SIGHTLINE_COUNT > 1)
    {
        angleStep =
            Config::SIGHTLINE_FOV_RADIANS /
            static_cast<float>(Config::SIGHTLINE_COUNT - 1);
    }

    for (int i = 0; i < Config::SIGHTLINE_COUNT; i++)
    {
        float angle = startAngle + angleStep * static_cast<float>(i);
        sightlines.push_back(castSingleSightline(angle));
    }

    return sightlines;
}

float Simulation::castSingleSightline(float angleRadians) const
{
    Vec2 rayOrigin = bulldozer.position;
    Vec2 rayDirection(std::cos(angleRadians), std::sin(angleRadians));

    float closestDistance = std::numeric_limits<float>::max();
    float detectedValue = -1.0f;

    for (const Circle& circle : circles)
    {
        Vec2 toCircle = circle.position - rayOrigin;

        float projection = toCircle.dot(rayDirection);

        if (projection < 0.0f || projection > Config::SIGHTLINE_LENGTH)
        {
            continue;
        }

        Vec2 closestPoint =
            rayOrigin + rayDirection * projection;

        Vec2 difference =
            circle.position - closestPoint;

        float distanceSquared = difference.lengthSquared();

        if (distanceSquared <= circle.radius * circle.radius)
        {
            if (projection < closestDistance)
            {
                closestDistance = projection;

                if (circle.material == CircleMaterial::GraphiteRodEnd)
                {
                    detectedValue = 1.0f;
                }
                else
                {
                    detectedValue = 0.0f;
                }
            }
        }
    }

    return detectedValue;
}

Simulation::Simulation()
    : clearedGraphiteCount(0)
{
}

bool Simulation::setNeuralWeights(const std::vector<float>& weights)
{
    return neuralNetwork.setFromFlatWeights(weights);
}

int Simulation::getClearedGraphiteCount() const
{
    return clearedGraphiteCount;
}

int Simulation::getRemainingGraphiteCount() const
{
    int count = 0;

    for (const Circle& circle : circles)
    {
        if (circle.material == CircleMaterial::GraphiteRodEnd)
        {
            count++;
        }
    }

    return count;
}


const std::vector<Circle>& Simulation::getCircles() const
{
    return circles;
}

bool Simulation::loadNeuralNetwork(const std::string& csvPath)
{
    return neuralNetwork.loadFromCSV(csvPath);
}


void Simulation::reset()
{
    circles.clear();
    clearedGraphiteCount = 0;
        bulldozer.reset();
    generateRubble();
}
const Bulldozer& Simulation::getBulldozer() const
{
    return bulldozer;
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

void Simulation::generateRubble()
{
    circles.clear();
    circles.reserve(Config::RUBBLE_COUNT);

    int attempts = 0;
    const int maxAttempts = Config::RUBBLE_COUNT * 100;

    int graphitePlaced = 0;

    while (static_cast<int>(circles.size()) < Config::RUBBLE_COUNT &&
           attempts < maxAttempts)
    {
        attempts++;

        const bool isGraphite =
            graphitePlaced < Config::GRAPHITE_NUMBER;

        const CircleMaterial material =
            isGraphite
            ? CircleMaterial::GraphiteRodEnd
            : CircleMaterial::ConcreteRubble;

        float radius;

        if (isGraphite)
        {
            radius = randomFloat(
                Config::MIN_GRAPHITE_RADIUS,
                Config::MAX_GRAPHITE_RADIUS
            );
        }
        else
        {
            radius = randomFloat(
                Config::MIN_RUBBLE_RADIUS,
                Config::MAX_RUBBLE_RADIUS
            );
        }

        float x = randomFloat(
            Config::SIMULATION_LEFT + radius,
            Config::SIMULATION_RIGHT - radius
        );

        float y = randomFloat(
            Config::SIMULATION_BOTTOM + radius,
            Config::SIMULATION_TOP - radius
        );

        Circle candidate;

        candidate.position = Vec2(x, y);

        candidate.velocity = Vec2(
            randomFloat(-Config::INITIAL_SPEED, Config::INITIAL_SPEED),
            randomFloat(-Config::INITIAL_SPEED, Config::INITIAL_SPEED)
        );

        candidate.radius = radius;
        candidate.material = material;

        float density =
            isGraphite
            ? Config::GRAPHITE_DENSITY
            : Config::CONCRETE_DENSITY;

        candidate.mass = radius * radius * density;

        bool overlaps = false;

        for (const Circle& other : circles)
        {
            Vec2 difference = candidate.position - other.position;
            float minDistance = candidate.radius + other.radius;

            if (difference.lengthSquared() < minDistance * minDistance)
            {
                overlaps = true;
                break;
            }
        }

        if (!overlaps)
        {
            circles.push_back(candidate);

            if (isGraphite)
            {
                graphitePlaced++;
            }
        }

    }

      std::random_device rd;
    
    // Initialize the generator with the seed
    std::mt19937 g(rd());
    std::shuffle(circles.begin(),circles.end(),g);

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
    if (circle.material == CircleMaterial::GraphiteRodEnd)
    {
        // Graphite is allowed to leave through the top.
        continue;
    }

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
