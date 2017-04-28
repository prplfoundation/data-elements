#!/bin/bash

# Test script to be called by wccf_util_update_worker.sh

# Reflash with the openwrt*bin image file sitting in /etc/config
# Fails silently if no image file is present there.

# Usage: /etc/config/reflash_ap.sh &
set -x

# Wait for caller script to clear out
sleep 10
if [ -f /etc/config/openwrt*bin ]; then

    # Must move file to /tmp to work properly
    mv -f /etc/config/openwrt*bin /tmp/

    sysupgrade -v /tmp/openwrt*bin
fi
