#include "NeuralNetwork.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Config.h"

NeuralNetwork::NeuralNetwork()
    : loaded(false)
{
}

bool NeuralNetwork::loadFromCSV(const std::string& filePath)
{
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        std::cerr << "Failed to open neural network CSV: "
                  << filePath << std::endl;

        loaded = false;
        return false;
    }

    std::vector<float> values;
    std::string line;

    while (std::getline(file, line))
    {
        std::stringstream lineStream(line);
        std::string cell;

        while (std::getline(lineStream, cell, ','))
        {
            if (!cell.empty())
            {
                values.push_back(std::stof(cell));
            }
        }
    }

    const int inputHiddenCount =
        Config::NETWORK_INPUT_COUNT * Config::NETWORK_HIDDEN_COUNT;

    const int hiddenBiasCount =
        Config::NETWORK_HIDDEN_COUNT;

    const int hiddenOutputCount =
        Config::NETWORK_HIDDEN_COUNT * Config::NETWORK_OUTPUT_COUNT;

    const int outputBiasCount =
        Config::NETWORK_OUTPUT_COUNT;

    const int expectedCount =
        inputHiddenCount +
        hiddenBiasCount +
        hiddenOutputCount +
        outputBiasCount;

    if (static_cast<int>(values.size()) != expectedCount)
    {
        std::cerr << "Neural network CSV has wrong number of values.\n";
        std::cerr << "Expected: " << expectedCount << "\n";
        std::cerr << "Got:      " << values.size() << std::endl;

        loaded = false;
        return false;
    }

    int index = 0;

    inputToHiddenWeights.assign(
        Config::NETWORK_HIDDEN_COUNT,
        std::vector<float>(Config::NETWORK_INPUT_COUNT, 0.0f)
    );

    hiddenBiases.assign(Config::NETWORK_HIDDEN_COUNT, 0.0f);

    hiddenToOutputWeights.assign(
        Config::NETWORK_OUTPUT_COUNT,
        std::vector<float>(Config::NETWORK_HIDDEN_COUNT, 0.0f)
    );

    outputBiases.assign(Config::NETWORK_OUTPUT_COUNT, 0.0f);

    for (int hidden = 0; hidden < Config::NETWORK_HIDDEN_COUNT; hidden++)
    {
        for (int input = 0; input < Config::NETWORK_INPUT_COUNT; input++)
        {
            inputToHiddenWeights[hidden][input] = values[index];
            index++;
        }
    }

    for (int hidden = 0; hidden < Config::NETWORK_HIDDEN_COUNT; hidden++)
    {
        hiddenBiases[hidden] = values[index];
        index++;
    }

    for (int output = 0; output < Config::NETWORK_OUTPUT_COUNT; output++)
    {
        for (int hidden = 0; hidden < Config::NETWORK_HIDDEN_COUNT; hidden++)
        {
            hiddenToOutputWeights[output][hidden] = values[index];
            index++;
        }
    }

    for (int output = 0; output < Config::NETWORK_OUTPUT_COUNT; output++)
    {
        outputBiases[output] = values[index];
        index++;
    }

    loaded = true;

    std::cout << "Loaded neural network from: "
              << filePath << std::endl;
    
    if (!setFromFlatWeights(values))
{
    std::cerr << "Neural network CSV has wrong number of values.\n";
    std::cerr << "Expected: " << getWeightCount() << "\n";
    std::cerr << "Got:      " << values.size() << std::endl;
    return false;
}

std::cout << "Loaded neural network from: " << filePath << std::endl;


    return true;
}

std::vector<float> NeuralNetwork::forward(const std::vector<float>& inputs) const
{
    std::vector<float> outputs(Config::NETWORK_OUTPUT_COUNT, 0.0f);

    if (!loaded)
    {
        return outputs;
    }

    if (static_cast<int>(inputs.size()) != Config::NETWORK_INPUT_COUNT)
    {
        std::cerr << "Neural network input size mismatch.\n";
        return outputs;
    }

    std::vector<float> hidden(Config::NETWORK_HIDDEN_COUNT, 0.0f);

    for (int h = 0; h < Config::NETWORK_HIDDEN_COUNT; h++)
    {
        float sum = hiddenBiases[h];

        for (int i = 0; i < Config::NETWORK_INPUT_COUNT; i++)
        {
            sum += inputToHiddenWeights[h][i] * inputs[i];
        }

        hidden[h] = activation(sum);
    }

    for (int o = 0; o < Config::NETWORK_OUTPUT_COUNT; o++)
    {
        float sum = outputBiases[o];

        for (int h = 0; h < Config::NETWORK_HIDDEN_COUNT; h++)
        {
            sum += hiddenToOutputWeights[o][h] * hidden[h];
        }

        outputs[o] = activation(sum);
    }

    return outputs;
}

bool NeuralNetwork::isLoaded() const
{
    return loaded;
}

float NeuralNetwork::activation(float value) const
{
    return std::tanh(value);
}



int NeuralNetwork::getWeightCount()
{
    return
        Config::NETWORK_INPUT_COUNT * Config::NETWORK_HIDDEN_COUNT +
        Config::NETWORK_HIDDEN_COUNT +
        Config::NETWORK_HIDDEN_COUNT * Config::NETWORK_OUTPUT_COUNT +
        Config::NETWORK_OUTPUT_COUNT;
}

bool NeuralNetwork::setFromFlatWeights(const std::vector<float>& values)
{
    if (static_cast<int>(values.size()) != getWeightCount())
    {
        return false;
    }

    int index = 0;

    inputToHiddenWeights.assign(
        Config::NETWORK_HIDDEN_COUNT,
        std::vector<float>(Config::NETWORK_INPUT_COUNT, 0.0f)
    );

    hiddenBiases.assign(Config::NETWORK_HIDDEN_COUNT, 0.0f);

    hiddenToOutputWeights.assign(
        Config::NETWORK_OUTPUT_COUNT,
        std::vector<float>(Config::NETWORK_HIDDEN_COUNT, 0.0f)
    );

    outputBiases.assign(Config::NETWORK_OUTPUT_COUNT, 0.0f);

    for (int h = 0; h < Config::NETWORK_HIDDEN_COUNT; h++)
    {
        for (int i = 0; i < Config::NETWORK_INPUT_COUNT; i++)
        {
            inputToHiddenWeights[h][i] = values[index++];
        }
    }

    for (int h = 0; h < Config::NETWORK_HIDDEN_COUNT; h++)
    {
        hiddenBiases[h] = values[index++];
    }

    for (int o = 0; o < Config::NETWORK_OUTPUT_COUNT; o++)
    {
        for (int h = 0; h < Config::NETWORK_HIDDEN_COUNT; h++)
        {
            hiddenToOutputWeights[o][h] = values[index++];
        }
    }

    for (int o = 0; o < Config::NETWORK_OUTPUT_COUNT; o++)
    {
        outputBiases[o] = values[index++];
    }

    loaded = true;
    return true;
}

std::vector<float> NeuralNetwork::getFlatWeights() const
{
    std::vector<float> values;

    for (const auto& row : inputToHiddenWeights)
        for (float v : row)
            values.push_back(v);

    for (float v : hiddenBiases)
        values.push_back(v);

    for (const auto& row : hiddenToOutputWeights)
        for (float v : row)
            values.push_back(v);

    for (float v : outputBiases)
        values.push_back(v);

    return values;
}
