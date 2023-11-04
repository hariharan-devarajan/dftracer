#!/bin/bash

folder=$1
dest=$2

echo "[" > $dest
gzip -c -d $folder/*.gz | grep -v "\["   | awk '{$1=$1;print}' >> $dest
cat $folder/*.pfw | grep -v "\["   | awk '{$1=$1;print}' >> $dest
gzip -c -d $folder/.*.gz | grep -v "\["   | awk '{$1=$1;print}' >> $dest
cat $folder/.*.pfw | grep -v "\["   | awk '{$1=$1;print}' >> $dest
