#!/bin/bash

for f in *.txt; 
do 
    echo "Processing $f file.."; 
    echo ./Assignment3.exe $f 2
    ./Assignment3.exe $f 2
    
done
