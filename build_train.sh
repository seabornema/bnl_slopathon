#!/usr/bin/env bash

set -e

CXX=g++
CXXFLAGS="-std=c++17 -Wall -Wextra -pedantic -O3"

HEADER_DIR="include/header"
SOURCE_DIR="include/source"

OUTPUT="train_bulldozer"

SOURCES="
train.cpp
${SOURCE_DIR}/Circle.cpp
${SOURCE_DIR}/Simulation.cpp
${SOURCE_DIR}/Bulldozer.cpp
${SOURCE_DIR}/NeuralNetwork.cpp
"

INCLUDES="-I${HEADER_DIR} -Iinclude"

echo "Building ${OUTPUT} with -O3..."

${CXX} ${CXXFLAGS} ${INCLUDES} ${SOURCES} -o ${OUTPUT}

echo "Build complete."
echo "Run with: ./${OUTPUT}"
