#!/bin/bash
g++ -o topology generate_topology.cpp -std=c++14 -I .
echo "compiled"
./topology 7 13
echo "Done"