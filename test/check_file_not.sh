#!/bin/bash
expected_lines=$2
if ls $1 1> /dev/null 2>&1; then
    ls $1
    rm -r $1 2> /dev/null
    exit 1
fi