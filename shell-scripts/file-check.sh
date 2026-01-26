#!/bin/bash
read -p "Enter the filename to check: " FILENAME

if [ -f "$FILENAME" ]; then
    echo "Success: $FILENAME exists."
    ls -l "$FILENAME"
else
    echo "Error: $FILENAME does not exist."
fi
