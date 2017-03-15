#!/bin/bash

# Simple script to scan sensor output (all sensors) and verify
# that json output has correct keys (as in key-value data).
# Visual inspection of result is required.

# Sample usage scenario:
#
#    ./sensor_field_check.sh /home/abccf/git-local-wccf/wccf/qa/temp > ~/fieldcheck.out
#    vi ~/fieldcheck.out
#

if [ $# -ne 1 ]; then
    echo "  usage: ./sensor_field_check.sh <sensor_output_path>"
    echo "example: ./sensor_field_check.sh /home/abccf/git-local-wccf/wccf/qa/temp"
    exit 1
fi 

JSON_OUTPUT_SOURCE_DIR=$1

echo "Using JSON_OUTPUT_SOURCE_DIR: $JSON_OUTPUT_SOURCE_DIR"

function do_field_check ()
{
    ### echo "In do_field_check( $1, $2, $3)"

    P_FILENM=$1
    P_SENSOR=$2
    P_FIELD=$3
    P_RELEASE=$4

    echo "__________________________________________________________________________________"
    echo "__________________________________________________________________________________"
    echo "For Sensor: $P_SENSOR"

    TOTAL_COUNT=0

    # Looks at EVERY file in source dir -
    for FILE in `ls -1 $JSON_OUTPUT_SOURCE_DIR/*${P_FILENM}*`; do
        echo ""
        echo "====="
        echo "File: $FILE"
        echo "Field: $P_FIELD, Release Debut: $P_RELEASE"
        echo ""
        COUNT=`cat $FILE | grep \"$P_FIELD\" | wc -l`
        echo "Count = $COUNT"
        if [ $COUNT -eq 0 ]; then
            echo "MISSING"
        fi
        ((TOTAL_COUNT = TOTAL_COUNT+COUNT))

    done

    echo ""
    echo "Total count for $P_FIELD: $TOTAL_COUNT"
    ### sleep 10
    
}

# Planned Sensor Output Fields as of March 1, 2017

do_field_check channel Channel APIfIndex R1
do_field_check channel Channel APMACAddress R1
do_field_check channel Channel APName R2
do_field_check channel Channel Capability R2
do_field_check channel Channel ChannelActiveTime R1
do_field_check channel Channel ChannelBusyTime R1
do_field_check channel Channel ChannelNumber R2
do_field_check channel Channel ChannelReceiveTime R1
do_field_check channel Channel ChannelTransmitTime R1
do_field_check channel Channel CurrentUTCTime R1
do_field_check channel Channel Frequency R1
do_field_check channel Channel InUse R1
do_field_check channel Channel Noise R1
do_field_check interface Interface APIfIndex R2
do_field_check interface Interface APMACAddress R2
do_field_check interface Interface APName R2
do_field_check interface Interface CenterFreq1 R2
do_field_check interface Interface CenterFreq2 R2
do_field_check interface Interface ChannelWidth R2
do_field_check interface Interface CurrentUTCTime R2
do_field_check interface Interface HTChannelType R2
do_field_check scan Scan APIfIndex R1
do_field_check scan Scan APMACAddress R1
do_field_check scan Scan APName R1
do_field_check scan Scan ChannelNumber R1
do_field_check scan Scan ChannelUtil R3
do_field_check scan Scan CurrentUTCTime R1
do_field_check scan Scan Frequency R2
do_field_check scan Scan RemoteMACAddress R1
do_field_check scan Scan Signal R1
do_field_check scan Scan SSID R1
do_field_check scan Scan StationCnt R3
do_field_check station Station APIfIndex R1
do_field_check station Station APMACAddress R1
do_field_check station Station APName R1
do_field_check station Station Capability R2
do_field_check station Station CurrentUTCTime R1
do_field_check station Station HtRxMCS R1
do_field_check station Station HtTxMCS R1
do_field_check station Station InactiveTime R1
do_field_check station Station IPv4Address R3
do_field_check station Station MACAddress R1
do_field_check station Station RxBytes R1
do_field_check station Station RxPackets R1
do_field_check station Station Signal R1
do_field_check station Station SignalAvg R1
do_field_check station Station StationName R3
do_field_check station Station TxBitrate R1
do_field_check station Station RxBitrate R1
do_field_check station Station TxBytes R1
do_field_check station Station TxFailed R1
do_field_check station Station TxPackets R1
do_field_check station Station TxRetries R1
do_field_check station Station VhtRxMCS R2
do_field_check station Station VhtTxMCS R2
do_field_check station Station VhtRxNSS R2
do_field_check station Station VhtTxNSS R2
do_field_check station Station VhtTxShortGI R2
do_field_check station Station VhtRxShortGI R2



