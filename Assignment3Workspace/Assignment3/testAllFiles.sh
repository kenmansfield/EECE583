#!/bin/bash

for f in *.txt; 
do 
    echo "Processing $f file.."; 
    echo ./Assignment3.exe $f 1
    ./Assignment3.exe $f 1
    
done
