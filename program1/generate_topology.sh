#!/bin/bash
g++ generate_topology.cpp -std=c++14 -I .
echo "compiled"
./a.out 7 11