#!/bin/bash

echo "Sensor Output" > ~/catalljson.out
for FILE in `ls *.json`; do
  echo "" >> ~/catalljson.out
  echo "FILE: $FILE" >> ~/catalljson.out
  cat $FILE >> ~/catalljson.out
  echo "" >> ~/catalljson.out
done
