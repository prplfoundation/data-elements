#!/bin/bash

# Test script to be called by wccf_util_update_worker.sh

# Delay reboot by the provided num of seconds

# Usage: /etc/config/delayed_reboot.sh [ <secs_delay> ] &

SECONDS=20

if [ $# -eq 1 ]; then
    SECONDS=$1
fi

reboot -d $1
