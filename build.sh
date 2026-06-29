#!/usr/bin/env bash

set -e

CXX=g++
CXXFLAGS="-std=c++17 -Wall -Wextra -pedantic -O2"

OUTPUT="chernobyl_roof_sim"

HEADER_DIR="include/header"
SOURCE_DIR="include/source"

SOURCES="
main.cpp
${SOURCE_DIR}/Circle.cpp
${SOURCE_DIR}/Simulation.cpp
${SOURCE_DIR}/Render.cpp
${SOURCE_DIR}/InputHandler.cpp
${SOURCE_DIR}/glad.c
"

INCLUDES="-I${HEADER_DIR} -Iinclude"

LIBS="-lglfw -ldl -lGL"

echo "Building ${OUTPUT}..."

${CXX} ${CXXFLAGS} ${INCLUDES} ${SOURCES} ${LIBS} -o ${OUTPUT}

echo "Build complete."
echo "Run with: ./${OUTPUT}"
