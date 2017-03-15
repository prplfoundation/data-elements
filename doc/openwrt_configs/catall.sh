#!/bin/bash

echo "Running catall.sh"


for FILE in `find . -type f -name "*"`; do
	echo "================================================"
	echo ""
	echo "Backed up Config File: $FILE"
	echo ""
	cat $FILE
	echo ""
done
