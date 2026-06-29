#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <string>
#include <vector>

class NeuralNetwork
{
public:
    static int getWeightCount();
bool setFromFlatWeights(const std::vector<float>& values);
std::vector<float> getFlatWeights() const;
    NeuralNetwork();

    bool loadFromCSV(const std::string& filePath);

    std::vector<float> forward(const std::vector<float>& inputs) const;

    bool isLoaded() const;

private:
    bool loaded;

    std::vector<std::vector<float>> inputToHiddenWeights;
    std::vector<float> hiddenBiases;

    std::vector<std::vector<float>> hiddenToOutputWeights;
    std::vector<float> outputBiases;

    float activation(float value) const;
};

#endif
