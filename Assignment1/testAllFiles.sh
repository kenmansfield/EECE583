#!/bin/bash

for f in *.infile; 
do 
    echo "Processing $f file.."; 
    echo ./assignment1.exe $f 3
    ./assignment1.exe $f 3
    
done
