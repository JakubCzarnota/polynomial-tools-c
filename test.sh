#!/bin/sh
set -e

BUILD_DIR=build

# Configure
cmake -S . -B "$BUILD_DIR"

# Build
cmake --build "$BUILD_DIR"

# Run tests (no cd!)
echo "Running tests..."
ctest --test-dir "$BUILD_DIR" --verbose
