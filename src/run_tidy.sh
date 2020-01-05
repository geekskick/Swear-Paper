#!/bin/bash -e

clang-tidy -p ../build/ "-checks=-*,modernize-*,misc-*,performance-*,bugprone-*,readability-*,cppcoreguidelines-*" * "-header-filter=.*/src/.*hpp" &> tidy.output

echo "Written to tidy.output"
