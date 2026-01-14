#!/bin/sh
set -e

BUILD_DIR=build
APP_NAME=polynomial_app

# Configure (idempotent)
cmake -S . -B "$BUILD_DIR"

# Build
cmake --build "$BUILD_DIR"

# Run application
echo "Running application..."
"$BUILD_DIR/$APP_NAME"
