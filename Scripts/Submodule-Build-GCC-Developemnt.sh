#!/bin/bash
# =========================
# Build using GCC
# =========================

# Go to the directory of this script
cd "$(dirname "$0")"
cd ..

# Run CMake to configure the build
cmake -B Build/GCC -S . -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Development -DUSE_ENGINE_SUBMODULE="ON"

# Build the target
cmake --build Build/GCC --target PongTerminal

echo
echo "Build with GCC completed."
read -p "Press Enter to continue..."