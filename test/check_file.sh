#!/bin/bash
expected_lines=$2
echo "cat $1 | grep POSIX | wc -l"
num_posix_lines=$(cat $1 | wc -l)
echo $num_posix_lines
if [[ "$num_posix_lines" -lt "$expected_lines" ]]; then
  cat $1
  exit 1
fi
rm $1