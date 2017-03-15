#!/bin/bash
#
# Copyright (c) 2017 Applied Broadband, Inc., and
#                    Cable Television Laboratories, Inc. ("CableLabs")
#
# This script is designed to be run on an OpenWrt AP with WCCF installed.
#
# Place this file in /etc/config/ to survive reflashing when 
# you've selected to have the config saved through reflash.
# If you choose a different location, update the script
# location variable (SCRIPT_HOME) in wccf_qa_do_all_sensors.sh.

# NOTE: This script assumes wccf, iw and iwinfo are
# all available on the AP.

TMP_DIR=$1

echo "iw dev wlan0 station dump" >> $TMP_DIR/iw_station_dump_wlan0
iw dev wlan0 station dump >> $TMP_DIR/iw_station_dump_wlan0

wccf_sensor_station --interface-name=wlan0 --output-dir=$TMP_DIR


echo "iw dev wlan1 station dump" >> $TMP_DIR/iw_station_dump_wlan1
iw dev wlan1 station dump >> $TMP_DIR/iw_station_dump_wlan1

wccf_sensor_station --interface-name=wlan1 --output-dir=$TMP_DIR


echo "iw dev wlan0 survey dump" >> $TMP_DIR/iw_survey_dump_wlan0
iw dev wlan0 survey dump >> $TMP_DIR/iw_survey_dump_wlan0

wccf_sensor_channel --interface-name=wlan0 --output-dir=$TMP_DIR


echo "iw dev wlan1 survey dump" >> $TMP_DIR/iw_survey_dump_wlan1
iw dev wlan1 survey dump >> $TMP_DIR/iw_survey_dump_wlan1

wccf_sensor_channel --interface-name=wlan1 --output-dir=$TMP_DIR


echo "iwinfo wlan0 scan" >> $TMP_DIR/iwinfo_scan_wlan0
iwinfo wlan0 scan >> $TMP_DIR/iwinfo_scan_wlan0

wccf_sensor_scan --interface-name=wlan0 --output-dir=$TMP_DIR


echo "iwinfo wlan1 scan" >> $TMP_DIR/iwinfo_scan_wlan1
iwinfo wlan1 scan >> $TMP_DIR/iwinfo_scan_wlan1

wccf_sensor_scan --interface-name=wlan1 --output-dir=$TMP_DIR
