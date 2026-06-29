#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include "Simulation.h"
#include "NeuralNetwork.h"

struct Genome
{
    std::vector<float> weights;
    float score;
};

static float randomFloat(std::mt19937& rng, float min, float max)
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

static Genome createRandomGenome(std::mt19937& rng)
{
    Genome genome;
    genome.score = 0.0f;

    int weightCount = NeuralNetwork::getWeightCount();
    genome.weights.resize(weightCount);

    for (float& weight : genome.weights)
    {
        weight = randomFloat(rng, -1.0f, 1.0f);
    }

    return genome;
}

static Genome mutateGenome(
    const Genome& parent,
    std::mt19937& rng,
    float mutationRate,
    float mutationStrength
)
{
    Genome child;
    child.weights = parent.weights;
    child.score = 0.0f;

    std::uniform_real_distribution<float> chance(0.0f, 1.0f);
    std::normal_distribution<float> mutation(0.0f, mutationStrength);

    for (float& weight : child.weights)
    {
        if (chance(rng) < mutationRate)
        {
            weight += mutation(rng);
            weight = std::clamp(weight, -5.0f, 5.0f);
        }
    }

    return child;
}

static float evaluateGenome(
    const Genome& genome,
    int seed,
    float simulationSeconds,
    float timestep
)
{
    std::srand(seed);

    Simulation simulation;
    simulation.setNeuralWeights(genome.weights);
    simulation.initialize();

    int steps = static_cast<int>(simulationSeconds / timestep);

    for (int i = 0; i < steps; i++)
    {
        simulation.update(timestep);
    }

    int cleared = simulation.getClearedGraphiteCount();
    int remaining = simulation.getRemainingGraphiteCount();

    return
        cleared * 1000.0f -
        remaining * 5.0f;
}

static void saveWeightsCSV(
    const std::string& path,
    const std::vector<float>& weights
)
{
    std::ofstream file(path);

    for (std::size_t i = 0; i < weights.size(); i++)
    {
        file << weights[i];

        if (i + 1 < weights.size())
        {
            file << ",";
        }
    }

    file << "\n";
}

int main()
{
    const int populationSize = 120;
    const int eliteCount = 12;
    const int generations = 4000;

    const float simulationSeconds = 15.0f;
    const float timestep = 1.0f / 60.0f;

    const float mutationRate = 0.08f;
    const float mutationStrength = 0.25f;

    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));

    std::vector<Genome> population;

    for (int i = 0; i < populationSize; i++)
    {
        population.push_back(createRandomGenome(rng));
    }

    for (int generation = 0; generation < generations; generation++)
    {
        for (Genome& genome : population)
        {
            float totalScore = 0.0f;

            // Multiple seeds prevents overfitting to one rubble layout.
            for (int trial = 0; trial < 3; trial++)
            {
                int seed = 1000 + trial;
                totalScore += evaluateGenome(
                    genome,
                    seed,
                    simulationSeconds,
                    timestep
                );
            }

            genome.score = totalScore / 3.0f;
        }

        std::sort(
            population.begin(),
            population.end(),
            [](const Genome& a, const Genome& b)
            {
                return a.score > b.score;
            }
        );

float averageScore = 0.0f;

for (const Genome& genome : population)
{
    averageScore += genome.score;
}

averageScore /= static_cast<float>(population.size());

std::cout << "Generation "
          << generation
          << " average score: "
          << averageScore
          << " best score: "
          << population[0].score
          << std::endl;

        std::vector<Genome> nextPopulation;

        for (int i = 0; i < eliteCount; i++)
        {
            nextPopulation.push_back(population[i]);
        }

        while (static_cast<int>(nextPopulation.size()) < populationSize)
        {
            int parentIndex = static_cast<int>(
                randomFloat(rng, 0.0f, static_cast<float>(eliteCount))
            );

            if (parentIndex >= eliteCount)
            {
                parentIndex = eliteCount - 1;
            }

            Genome child = mutateGenome(
                population[parentIndex],
                rng,
                mutationRate,
                mutationStrength
            );

            nextPopulation.push_back(child);
        }

        population = nextPopulation;

        if (generation % 10 == 0)
        {
            saveWeightsCSV("best_weights.csv", population[0].weights);
        }
    }

    saveWeightsCSV("weights_2.csv", population[0].weights);

    std::cout << "Training complete.\n";
    std::cout << "Saved best model to weights.csv\n";

    return 0;
}
