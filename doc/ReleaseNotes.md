#WCCF Release Notes

##Version 1.3.0 - March 15, 2017

###General Issues
- Renamed wccf\_sensor\_survey to wccf\_sensor\_channel for clarity
- Fixed problem with wccf\_sensor\_channel only reporting wlan0 correctly
- Created new sensor wccf\_sensor\_interface to report on channel and frequency configuration

###Added new data fields
- Added Frequency parameter to wccf\_sensor\_scan
- Added VhtTxMCS parameter to wccf\_sensor\_station
- Added VhtRxNSS parameter to wccf\_sensor\_station
- Added VhtRxShortGI parameter to wccf\_sensor\_station
- Added ChannelNumber parameter to wccf\_sensor\_channel
- Changed Channel to ChannelNumber in wccf\_sensor\_scan
- Added ChannelBusyExtTime parameter to wccf\_sensor\_channel

###Support for new Access Point Device
- TP-Link AC1750 Model Archer C7 v2 dual band with 5GHz 802.11a/n/ac capability

###Cron.update Removed
This file was unnecessary for proper installation of a crontab so was removed.

###WCCF Test Build Utilities
Directory `wccf/qa/utilities` contains additional utilities to build WCCF and the OpenWrt image (Netgear or TP-LINK) with pre-defined .config files.  This means that the build and image creation, once started, completes without further human interaction.  In particular the 'make menuconfig' step is eliminated.  See comments in the scripts for details.

###Sample Sensor Files Renamed
Filenames with the colon character (":") origininating from related MAC addresses previously existed in this directory.  They caused problems when cloning the repo using Git for Windows.

The names were changed to remove time and address-sensitive information from the file names to support version coherent updates with new software releases.

Before:

    SampleSensor_44:94:fc:73:55:cd_scan-wlan0_2017013023400200.json
    SampleSensor_44:94:fc:73:55:cd_station-wlan0_2017013017550000.json
    SampleSensor_44:94:fc:73:55:cd_survey-wlan0_2017013023550100.json

After:

    Sample_wccf_sensor_scan.json
    Sample_wccf_sensor_station.json
    Sample_wccf_sensor_channel.json
##Version 1.2.0

Initial Cable Labs release
