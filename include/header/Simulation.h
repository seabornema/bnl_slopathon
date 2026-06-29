#ifndef SIMULATION_H
#define SIMULATION_H

#include <string>
#include <vector>
#include <random>

#include "Bulldozer.h"
#include "Circle.h"
#include "Config.h"
#include "NeuralNetwork.h"

class Simulation
{
public:
    Simulation();

    void initialize();
    void update(float deltaTime);

    bool loadNeuralNetwork(const std::string& csvPath);

    const std::vector<Circle>& getCircles() const;
    const Bulldozer& getBulldozer() const;

    void reset();
    bool setNeuralWeights(const std::vector<float>& weights);
int getClearedGraphiteCount() const;
int getRemainingGraphiteCount() const;

private:
int clearedGraphiteCount;
    std::vector<Circle> circles;
    Bulldozer bulldozer;
    NeuralNetwork neuralNetwork;

    void generateRubble();
    void integrate(float deltaTime);
    void applyFriction(float deltaTime);

    void resolveCollisions();
    void resolveBoundaryCollisions();

    void updateBulldozer(float deltaTime);
    void resolveBulldozerCollisions(float deltaTime);

    void removeClearedGraphite();

    std::vector<float> buildNetworkInputs() const;
    std::vector<float> castSightlines() const;
    float castSingleSightline(float angleRadians) const;

    float randomFloat(float min, float max) const;
};

#endif
