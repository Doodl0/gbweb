#!/usr/bin/env bash

cmake cmake -S . -B build-desktop -DCMAKE_BUILD_TYPE=Release
cmake --build build-desktop
