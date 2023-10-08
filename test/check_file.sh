#!/bin/bash
echo "cat $1 | grep POSIX | wc -l"
num_posix_lines=$(cat $1 | wc -l)
echo $num_posix_lines
if [[ "$num_posix_lines" == '0' ]]; then
  exit 1
fi