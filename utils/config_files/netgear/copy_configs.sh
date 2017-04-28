#!/bin/bash

# File: netgear/copy_configs.sh

# After flashing the AP with a WCCF-enabled image, these files are
# transferred to the AP with (note, two destinations accessed):

scp dropbear wireless network firewall root@192.168.1.1:/etc/config/
scp shadow root@192.168.1.1:/etc/
