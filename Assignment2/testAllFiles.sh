#!/bin/bash

for f in *.txt; 
do 
    echo "Processing $f file.."; 
    echo ./Assignment2.exe $f 3 1
    ./Assignment2.exe $f 3 1
    
done
