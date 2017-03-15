#!/bin/bash

# Format the sensor json output files
# File: format_json_output.sh

TARGET_DIR=$1
FORMATTED_OUTPUT=formatted_output

cd $TARGET_DIR

rm -f $FORMATTED_OUTPUT
echo "Formatted Output from $TARGET_DIR" > $FORMATTED_OUTPUT
echo "" >> $FORMATTED_OUTPUT

for FILE in `ls *.json`; do
  echo "======================================================================================" \
    >> $FORMATTED_OUTPUT
  echo "FILE: $FILE" >> $FORMATTED_OUTPUT
  python -mjson.tool $FILE >> $FORMATTED_OUTPUT 
done
echo "======================================================================================" \
    >> $FORMATTED_OUTPUT
