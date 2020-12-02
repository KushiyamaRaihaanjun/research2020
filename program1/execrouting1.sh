#!/bin/bash
g++ -o topology generate_topology.cpp -std=c++14 -I .
echo "topology compiled"
./topology 7 13
echo "Done"
g++ -o routing1 routing1.cpp -std=c++14 -I .
./routing1