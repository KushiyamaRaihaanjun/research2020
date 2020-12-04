#!/bin/bash
rm topology.txt
g++ -o topology generate_topology.cpp -std=c++14 -I .
echo "topology compiled"
./topology 50 200
echo "Done"
g++ -o routing1 -g3 routing1.cpp -std=c++14 -I .
./routing1