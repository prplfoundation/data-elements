#!/bin/bash 
#
# Copyright (c) 2017 Applied Broadband, Inc., and
#                    Cable Television Laboratories, Inc. ("CableLabs")
#
# File: wccf/qa/utilities/full_build.sh
#
# Required env vars (sample settings appear here):
#
#    export WCCF_BUILD_ROOT=$HOME/wccf_git_clone/wccf
#    export OPENWRT_BUILD_ROOT=$HOME/openwrt_git_clone/openwrt
#
SCRIPT_NAME=$( basename "$0" )
echo -e "\nStarting script $SCRIPT_NAME."

if [ $# -ne 1 ]; then
    echo "  usage: ./full_build.sh [ NETGEAR | TP-LINK ]"
    exit 1
fi

TARGET_AP_TMP=$1
echo "$TARGET_AP_TMP"
TARGET_AP=`echo $TARGET_AP_TMP | tr [a-z] [A-Z]`
if [ "$TARGET_AP" == "NETGEAR" ]; then
    echo "Building for Netgear WNDR3800"
elif [ "$TARGET_AP" == "TP-LINK" ]; then
    echo "Building for TP-LINK Archer C7"
else
    echo "Cmd line parameter is: $TARGET_AP.  Must be NETGEAR or TP-LINK. Exiting."
    exit 2
fi

# Prepare the WCCF pieces
$WCCF_BUILD_ROOT/qa/utilities/build_wccf.sh 0

# Follow up with the OpenWrt image creation
if [ "$TARGET_AP" == "NETGEAR" ]; then
    $WCCF_BUILD_ROOT/qa/utilities/build_ap_image.sh $WCCF_BUILD_ROOT/qa/utilities/NETGEAR.config
elif [ "$TARGET_AP" == "TP-LINK" ]; then
    $WCCF_BUILD_ROOT/qa/utilities/build_ap_image.sh $WCCF_BUILD_ROOT/qa/utilities/TP-LINK.config
else
    echo "ERROR: OpenWrt BUILD FAILED.  NO IMAGE FILE CREATED."
fi

echo "Script $SCRIPT_NAME complete."

