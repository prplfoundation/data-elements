#!/bin/bash

# Test script to be called by wccf_util_update_worker.sh

# Update crontab with new input file, then delete input.

# Usage: /etc/config/cron_timeshift.sh <cron_file> &
set -x

# Wait for caller script to clear out
sleep 10

CRON_FILE=$1
if [ -f $CRON_FILE ]; then
    crontab $CRON_FILE
    rm -f $CRON_FILE
fi
