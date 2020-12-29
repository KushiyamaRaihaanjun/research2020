#!/bin/bash
g++ -o routing4 -g3 routing4.cpp -std=c++17 -I .
echo "complied"
echo "mode 0"
./routing4 0 1
echo "done"
echo "mode 1(wAttack)"
./routing4 1 5
./routing4 1 10
./routing4 1 15
./routing4 1 20
./routing4 1 25
./routing4 1 30
echo "done"
echo "mode 2 (with Attack with Trust)"
./routing4 2 5
./routing4 2 10
./routing4 2 15
./routing4 2 20
./routing4 2 25
./routing4 2 30
echo "done"
echo "mode 3 (with Attack with mymethod)"
./routing4 3 5
./routing4 3 10
./routing4 3 15
./routing4 3 20
./routing4 3 25
./routing4 3 30
echo "done"

echo "file copying..."
cp PDR-0-1.txt ../../grad_2020/figs/.

for i in {1..3} do
cp PDR-${i}-*.txt ../../grad_2020/figs/.
cp DETECT-${i}-*.txt ../../grad_2020/figs/.
done

echo "removing files"
cp PDR-0-1.txt ../../grad_2020/figs/.

for i in {1..3} do
rm PDR-${i}-*.txt 
rm DETECT-${i}-*.txt 
done

echo "Done"