#!/bin/bash

if [ "$#" -lt 2 ]
then
    echo "ERROR: Invalid number of arguments [Received: $#] [Expected: 2]"
    exit 1
fi

writefile=$1
writestr=$2

mkdir -p $(dirname "$writefile") && touch "$writefile"
if [ "$?" -ne 0 ]
then
    echo "ERROR: $writefile could not be created"
    exit 1
fi
echo "$writestr" > "$writefile"
