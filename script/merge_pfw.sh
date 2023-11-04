#!/bin/bash

folder=$1
dest=$2
d2=${dest}.bak
shopt -s dotglob
cat `echo $folder/*.pfw` >> $d2
gzip -c -d `echo $folder/*gz` >> $d2
grep -i "[^#[]" $d2 > $dest
printf '%s\n%s\n' "[" "$(cat ${dest})" > $dest
gzip $dest
rm $d2