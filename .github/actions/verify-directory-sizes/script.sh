#!/bin/bash

set -eu
set +x

# Bash script to check directory sizes, we want to keep each directory under
# at most 1000 entries so that the GitHub online interface can be used to
# browse without excluding files
MAX_DIR_SIZE=0
MAX_DIR_NAME=0

# This is enforced limit
DIR_SIZE_LIMIT=1000

check_directory () {
    local CURR_DIR=$1
    local CURR_DIR_SIZE=$(ls -l $CURR_DIR | wc -l)
    if [ "$CURR_DIR_SIZE" -gt "$DIR_SIZE_LIMIT" ]; then
        # Show the same message but in red
        echo -e "\033[0;31m$CURR_DIR has $CURR_DIR_SIZE entries\033[0m"
    else
        echo "$CURR_DIR has $CURR_DIR_SIZE entries"
    fi
    if [ "$CURR_DIR_SIZE" -gt "$MAX_DIR_SIZE" ]; then
        MAX_DIR_SIZE=$CURR_DIR_SIZE
        MAX_DIR_NAME=$CURR_DIR
    fi

    # Sending stderr to /dev/null is used to suppress errors about no such
    # file or directory when a directory has no sub directories
    local SUBDIRS=$(ls -d $CURR_DIR*/ 2>/dev/null)
    for SUBDIR in $SUBDIRS
    do
        check_directory $SUBDIR
    done
}

check_directory ./

echo "Biggest directory: $MAX_DIR_NAME"
echo "Size: $MAX_DIR_SIZE"

if [ "$MAX_DIR_SIZE" -gt "$DIR_SIZE_LIMIT" ]; then
    echo -e "\033[0;31mMaximum allowed size: $DIR_SIZE_LIMIT\033[0m"
    # Action is unsuccessful
    exit 1
fi
