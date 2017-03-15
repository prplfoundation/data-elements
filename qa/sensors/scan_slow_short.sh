#!/bin/bash

# Scan for neighboring APs slowly to see how 80211 library
# is really handling requests and responses.

SCAN_IF=wlan1
SCAN_REPITITIONS=25
SCAN_CLEAR_SECONDS=60
OUTPUT_DIR_BASE=/tmp/slow_scans

function do_scans() {

    SCAN_INTERVAL=$1
    DIR=$2

    OUTPUT_DIR=$OUTPUT_DIR_BASE/$DIR
    mkdir -p $OUTPUT_DIR

    echo "Scan Int: $SCAN_INTERVAL, Scan Dir: $OUTPUT_DIR"
    SCANS_LEFT=$SCAN_REPITITIONS
    while [ $SCANS_LEFT -gt 0 ] ; do
        echo "SCANS_LEFT: $SCANS_LEFT"
        ### wccf_sensor_scan  --interface-name=$SCAN_IF --output-dir=$OUTPUT_DIR
        iwinfo wlan1 scan > ${OUTPUT_DIR}/iwinfo_${SCANS_LEFT}.out
        let SCANS_LEFT=SCANS_LEFT-1
        sleep $SCAN_INTERVAL
    done
}

do_scans 0 zero_a

sleep 5
do_scans 0 zero_b

sleep 15
do_scans 0 zero_c

sleep $SCAN_CLEAR_SECONDS
do_scans 1 one

sleep $SCAN_CLEAR_SECONDS
do_scans 3 three

sleep $SCAN_CLEAR_SECONDS
do_scans 5 five

sleep $SCAN_CLEAR_SECONDS
do_scans 10 ten

sleep $SCAN_CLEAR_SECONDS
do_scans 15 fiften

### sleep $SCAN_CLEAR_SECONDS
### do_scans 30 thirty

### Done. What do we have?
ls -lR $OUTPUT_DIR_BASE
