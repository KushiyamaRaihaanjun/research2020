#!/bin/bash
rm topology.txt
g++ -o topology generate_topology.cpp -std=c++17 -I .
echo "topology compiled"
./topology 50 150
echo "Done"
echo "attacker set"
g++ -o attacker -g3 generate_attacker.cpp -std=c++17 -I .
./attacker 50 5
./attacker 50 10
./attacker 50 15
./attacker 50 20
./attacker 50 25
echo "Done"