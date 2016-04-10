#!/bin/bash

for f in netlists/*.txt; 
do 
    echo "Processing $f file.."; 
    echo project.exe $f 2 0 0
    time ./project.exe $f 2 0 0
    
done


for f in netlists/*.txt; 
do 
    echo "Processing $f file.."; 
    echo project.exe $f 2 0 1
    time ./project.exe $f 2 0 1
    
done

for f in netlists/*.txt; 
do 
    echo "Processing $f file.."; 
    echo project.exe $f 2 1 0
    time ./project.exe $f 2 1 0
    
done

for f in netlists/*.txt; 
do 
    echo "Processing $f file.."; 
    echo project.exe $f 2 1 1
    time ./project.exe $f 2 1 1
    
done
