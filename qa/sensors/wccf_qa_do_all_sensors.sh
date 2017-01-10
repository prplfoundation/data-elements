#!/bin/bash
#
# Copyright (c) 2017 Applied Broadband, Inc., and
#                    Cable Television Laboratories, Inc. ("CableLabs")
#
# This script is designed to be run on an OpenWrt AP with WCCF installed.
#
# Place this file in /etc/config/ to survive reflashing when 
# you've selected to have the config saved through reflash.
# If you choose a different location, update the next line.

SCRIPT_HOME=/etc/config

if [ $# -eq 1 ]; then
	OUT_FILE=$1
else
	OUT_FILE=/tmp/wccf_qa_all_sensors.out
fi

TMP_SENSOR_OUTPUT_DIR=/tmp/wccf_all_sensors

mkdir -p $TMP_SENSOR_OUTPUT_DIR
rm -rf $TMP_SENSOR_OUTPUT_DIR/*

# Do all at once to minimize clock advance throughout execution.
NOW=`date`
NOW_EPOCH=`date +%s`

$SCRIPT_HOME/wccf_qa_all_sensors.sh $TMP_SENSOR_OUTPUT_DIR

echo "#" > $OUT_FILE
echo "# Copyright (c) 2017 Cable Television Laboratories, Inc.  All rights reserved." > $OUT_FILE
echo "#" > $OUT_FILE
echo "Test wccf_qa_do_all_sensors" > $OUT_FILE
echo "Date: $NOW" >> $OUT_FILE
echo "Epoch Seconds: $NOW_EPOCH" >> $OUT_FILE
echo "" >> $OUT_FILE
for FILE in $(ls $TMP_SENSOR_OUTPUT_DIR); do
	echo "================================================" >> $OUT_FILE
	echo -e "\nFILE: $FILE" >> $OUT_FILE
	cat $TMP_SENSOR_OUTPUT_DIR/$FILE >> $OUT_FILE
	echo "" >> $OUT_FILE
done


