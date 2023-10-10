#!/bin/bash
expected_lines=$2
echo "cat $1 | grep POSIX | wc -l"
num_posix_lines=$(cat $1 | wc -l 2> /dev/null)
echo $num_posix_lines
rm -r $1 2> /dev/null
if [[ "$num_posix_lines" -lt "$expected_lines" ]]; then
  exit 1
fi