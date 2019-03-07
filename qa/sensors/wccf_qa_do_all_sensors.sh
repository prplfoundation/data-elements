#!/bin/bash
#

# Copyright (c) 2019 Cable Television Laboratories, Inc.
# Licensed under the BSD+Patent (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#          https://opensource.org/licenses/BSDplusPatent
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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
echo "# Copyright (c) 2017 Applied Broadband, Inc., and" > $OUT_FILE
echo "#                    Cable Television Laboratories, Inc. (\"CableLabs\")" > $OUT_FILE
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


