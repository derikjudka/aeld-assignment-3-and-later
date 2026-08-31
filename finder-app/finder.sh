#!/bin/bash

if [ "$#" -lt 2 ]
then
    echo "ERROR: Invalid number of arguments [Received: $#] [Expected: 2]"
    exit 1
fi

filesdir=$1
searchstr=$2

if [ ! -d "$filesdir" ]
then
    echo "ERROR: Parameter 1 \"$filesdir\" is not a directory"
    exit 1
fi

numfiles=$(find $filesdir -type f | wc -l)
numlines=$(grep -r $searchstr $filesdir | wc -l)
echo "The number of files are $numfiles and the number of matching lines are $numlines"
