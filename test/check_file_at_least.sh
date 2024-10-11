#!/bin/bash
expected_lines=$2
echo "cat $1 | grep POSIX | wc -l"
num_posix_lines=$(cat $1 | wc -l 2> /dev/null)
echo $num_posix_lines
if [[ "$num_posix_lines" -lt "$expected_lines" ]]; then
  cat $1
  rm -r $1 2> /dev/null
  exit 1
else
  if jq --slurp -e >/dev/null 2>&1 <<< `cat $1 | grep -v "\["  | grep -v "\]"  | awk '{$1=$1;print}'`; then
    echo "Parsed JSON successfully and got something other than false/null";
  else
    echo "Failed to parse JSON, or got false/null";
    jq --slurp -e <<< `cat $1 | grep -v "\[" | grep -v "\]"  | awk '{$1=$1;print}'`
    cat $1 | grep -v "\[" | grep -v "\]"  | awk '{$1=$1;print}'
    exit 1
  fi
fi
rm -r $1 2> /dev/null