# WCCF Release Notes

## Version 2.0.0 - December 7, 2018

### General Items
 - Added new report (Network) output that conforms where possible to Multi-AP Specification v1.0
 - Delete older WCCF reports after they have been merged into the new Network report
 - Removed wccf_proc_null processor from the default installation startup scripts
 - Upgraded OpenWRT version to 18.06.1
 - Added new TP-LINK Archer C7 v2 sysupgrade and factory images to repo at [wccf/qa/images/2.0.0](../qa/images/2.0.0)
 
### Known Issues
 - Not all parameters in Multi-AP Specification v1.0 as defined in Data_Elements_DRAFT_Specification_v0.0.16.pdf are supported
 - The software has only been tested on TP-LINK Archer C7 v2
 - Execution of sensors can be increased up to a frequency of once per 2 minutes, sub-minute frequencies is limited by crontab. In this 2 minute option, scan reports should be executed on even minutes and all other reports should be exeuted on the odd minutes.
 
## Version 1.4.1 - June 8, 2017

### General Items
- Added TP-LINK Archer C7 v2 sysupgrade and factory (vendor sw replacement) images to repo at [wccf/qa/images/1.4.1](../qa/images/1.4.1)
- Admin Guide updated to clarify what items are applicate to all releases (1.x), 1.4.x releases, and 1.4.1 releases.
- Crontab changed to 15-minute intervals (must force-update existing OpenWrt AP cronjobs with cmdline: # crontab /root/wccf_sensors.cron)
- Crontab for wccf_sensor_scan was changed to execute at 14,29,44,59 minutes to provide 14 minutes for SSID collection
- Fixed IfIndex, APName issues with PhyCapa sensor
- Fixed problem with wccf_sensor_event where STAMAC values were incorrect

####Known Issues:

- JIRA issue C3CCF-57 - disabling a radio kills its event sensor; reboot required to work around (or pid file deletion; service restart)

## Version 1.4.0 - April 30, 2017

### General Issues
- Added wccf_sensor_phycapa to report radio phy capabilities
- Added wccf_sensor_event to report Association Events from a new Station connection
- Updated the wccf_config_sync.sh script to restart the network when a change is detected instead of rebooting the system
- Added generalized Updater Capability to push new files or images to field-based APs (documented in [wccf/doc/AdminGuide.md](./AdminGuide.md), Section 6).

## Version 1.3.0 - March 15, 2017

### General Issues
- Renamed wccf\_sensor\_survey to wccf\_sensor\_channel for clarity
- Fixed problem with wccf\_sensor\_channel only reporting wlan0 correctly
- Created new sensor wccf\_sensor\_interface to report on channel and frequency configuration

### Added new data fields
- Added Frequency parameter to wccf\_sensor\_scan
- Added VhtTxMCS parameter to wccf\_sensor\_station
- Added VhtRxNSS parameter to wccf\_sensor\_station
- Added VhtRxShortGI parameter to wccf\_sensor\_station
- Added ChannelNumber parameter to wccf\_sensor\_channel
- Changed Channel to ChannelNumber in wccf\_sensor\_scan
- Added ChannelBusyExtTime parameter to wccf\_sensor\_channel

### Support for new Access Point Device
- TP-Link AC1750 Model Archer C7 v2 dual band with 5GHz 802.11a/n/ac capability

### Cron.update Removed
This file was unnecessary for proper installation of a crontab so was removed.

### WCCF Test Build Utilities
Directory `wccf/qa/utilities` contains additional utilities to build WCCF and the OpenWrt image (Netgear or TP-LINK) with pre-defined .config files.  This means that the build and image creation, once started, completes without further human interaction.  In particular the 'make menuconfig' step is eliminated.  See comments in the scripts for details.

### Sample Sensor Files Renamed
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

## Version 1.2.0

Initial CableLabs release
