#!/bin/bash
set -x

shopt -s globstar
clang-format -i **/*.cpp **/*.h **/*.hpp
