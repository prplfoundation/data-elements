#WCCF Admin Guide#
##Version 1.3.0.  March 15, 2017##

Copyright (c) 2017 Applied Broadband, Inc., and Cable Television Laboratories, Inc. ("CableLabs")

##Contents##

1. Overview
2. Installing WCCF Version 1.3.0
3. Configuring WCCF Version 1.3.0 Apps
    - i. Space Management
    - ii. Sensors
    - iii. Processors
    - iv. Transmitters
    - v. Receivers
4. Configuring the OpenWrt Device

##1. Overview##

Wi-Fi Common Collection Framework (WCCF), Version 1.3.0, targets OpenWrt-based Access Point (AP) hardware (https://openwrt.org/) running OpenWrt branch master (aka Designated Driver).  WCCF Version 1.3.0 has focused on supporting the following access point devices:

+ Netgear N600 Model WNDR3800 dual band with 2.4GHz 802.11b/g/n and 5GHz 802.11a/n,
+ TP-Link AC1750 Model Archer C7 dual band with 5GHz 802.11a/n/ac capability.

This document covers the details of configuring and operating WCCF applications within an OpenWrt Access Point image built with WCCF.  This document also addresses the configuration of a "Receiver" network element to which the WCCF-enabled Access Point will send its collected data.

The WCCF Version 1.3.0 release includes fully-functional system images ready to flash onto each of the Netgear and TP-Link devices (see above) for demonstration, testing, and production AP operation.  See Section 4, Configuring the OpenWrt Device, for details on AP configuration with these images.

Other technical documentation provided in the release covers the following topics:

+ WCCF HOWTO Docs Overview ([wccf/doc/HOWTO_start_here.md](./HOWTO_start_here.md))
+ WCCF Build Machine ([wccf/doc/HOWTO_build_machine.md](./HOWTO_build_machine.md))
+ Building WCCF ([wccf/doc/HOWTO_build_wccf.md](./HOWTO_build_wccf.md))
+ Building WCCF into OpenWrt ([wccf/doc/HOWTO_build_flashable_image.md](./HOWTO_build_flashable_image.md))
+ Samples of each Sensor's output content and format in the reponsitory [wccf/doc/](./).

##2. Installing WCCF Version 1.3.0##

The image files included with release 1.3.0 are OpenWrt "sysupgrade" format images.  This format is suitable to upgrade ("flash") an existing OpenWrt device.  It is not suitable for use as a "first installation" of OpenWrt onto a factory-image based access point.  Such a "first installation" image can be built following the steps detailed in the HOWTO documents above.

To install either provided image, one can either interact with the target device using its Web-based OpenWrt U/I, or one can scp the image onto the device, then ssh to the device and force the flash using the command line.

###Using the Web-based U/I to Reflash the Device###
Access the Access Point's OpenWrt Web U/I from any web browser.  You will need the root-user password for the device to log on.

Under the System menu, choose item "Backup / Flash Firmware" and then scroll down to "Flash new firmware image."

Decide whether to check the "Keep settings" box (**recommended**).  See "Reflashing using the Command Line", below, for tips on how to see exactly what will be preserved in a reflash if you choose "Keep settings", and how to interactively alter the specific behavior of "Keep settings".

If you do not select to keep settings, various AP values such as IP addresses and wireless settings likely will be lost (effectively like doing a firmware reset on any AP).  Additionally, the wireless radios themselves may be disabled when settings are lost.  Finally, the root password for ssh access may be lost, requiring you to telnet onto the device to reset the root password, thus restoring ssh access.  (*Note that these are all basic OpenWrt behaviors, not WCCF behaviors.  Consequently, it is recommended that you initially work locally, as a Station attached to the AP - wireless or wired - until such behaviors become familiar.*)

****
WARNING: Resetting the AP, especially if *not* saving settings, may cause loss of network access depending upon how you have connected to the device.  For example, the WAN port may revert to address 192.168.1.1 from a previously defined static address, or may revert from static to a prior, or new, DHCP-assigned IP address.  In such cases, your only alternative to reconnect may be directly through a LAN port on the AP (which may require physical access to the device).
****

Continuing using the Web U/I, using the Choose File browser button, select the desired image file (in this release it will be named openwrt-ar71xx-generic-\<profile\>-squashfs-sysupgrade.bin, where \<profile\> is wndr3800 or archer-c7-v2), then proceed with the re-flash by clicking the "Flash Image..." button, and finally, the "Proceed" button.  

Typically, the flash and reboot will take no more than 3-4 minutes.

###Reflashing using the Command Line###

Rather than using the OpenWrt Web U/I, the new image can be copied (scp) to the device and flashed using the command line.  SSH access to the AP is required for the second step in this process.  Note that to connect to the AP over its WAN port (vs a LAN port) the SSH process (called dropbear in OpenWrt) must be configured to allow SSH through that WAN interface (which is disabled by default), and the firewall process (called firewall) must have a rule to allow TCP through the WAN interface, port 22.  You may also wish to allow port 80 access (http) through the WAN interface, also.

For these changes to be made, access first must take place over a LAN port - either through SSH or via the Web U/I.  The following steps describe a command-line approach.

In file `/etc/config/firewall`, enable ssh access over WAN, and (*optionally*) http over WAN.  Add these sections to the file:

    config rule
        option target 'ACCEPT'
        option src 'wan'
        option proto 'tcp'
        option dest_port '22'
        option name 'Allow-SSH-WAN'
        
    config rule
        option target 'ACCEPT'
        option src 'wan'
        option proto 'tcp'
        option dest_port '80'
        option name 'Allow-HTTP-WAN'

Then, restart the firewall process:

    $ /etc/init.d/firewall restart

In file `/etc/config/dropbear`, include only one of the two option statements below to designate which interface allows SSH.  For support on both interfaces, **remove** both statements from the config (which would correspond to 'unspecified' in the Web U/I):

    option Interface 'lan'
    option Interface 'wan'

Then, restart the sshd process (dropbear):

    $ /etc/init.d/dropbear restart

At this point, the AP will allow HTTP and SSH access over the WAN interface.

Finally, from a remote (connected) computer enter these commands (where \<profile\> represents your Netgear or your TP-Link selection during the OpenWrt build):

    $ scp <image_file_loc>/openwrt-ar71xx-generic-<profile>-squashfs-sysupgrade.bin  root@<AccessPoint_IP>:
    $ ssh root@<AccessPoint_IP>

    root@OpenWrt:~# sysupgrade openwrt-ar71xx-generic-<profile>-squashfs-sysupgrade.bin

At this stage, the AP will load the new image and reboot with the same behavior as the above Web U/I upgrade with "Keep settings" selected.  If you wish to discard settings, or alter which settings are kept, you can view the help for command sysupgrade with the -h cmd line argument.  There you'll see additional options including 'do not save' and 'interactive mode' which offer options.

For more on command-line upgrade, see:

 - [https://wiki.openwrt.org/doc/techref/sysupgrade](https://wiki.openwrt.org/doc/techref/sysupgrade).
 - [https://wiki.openwrt.org/doc/howto/generic.sysupgrade](https://wiki.openwrt.org/doc/howto/generic.sysupgrade).

For an illustration of the above process on the Netgear (which is very similar to the TP-LINK process), you may view:

- [wccf/doc/HOWTO_FlashTheNetgearWNDR3800.md](./HOWTO_FlashTheNetgearWNDR3800.md)


##3. Configuring WCCF Version 1.3.0 Apps##

###i. Space Management###
Storage space generally is limited on OpenWrt devices.  WCCF apps use some storage space on the AP.  Generally, they configure space under /tmp for files only until they are transferred to a Receiver at another address.

App logging is initially configured to go to /root/log.  Although this location may be on a smaller partion than /tmp, content is preserved across reboots which is a benefit.

See the following sections for details on configuring file locations.

###ii. Sensors###

In WCCF, Version 1.3.0, Sensors exist that obtain Wi-Fi information and store it in files for subsequent delivery through a Transmitter to a remote Receiver.  In Version 1.3.0, crontabs are used to trigger the Sensors periodically (initially, at 5-minute intervals).  The WCCF Version 1.3.0 build/install package installs a cronjob for user root with pre-configured sensor entries.  This cronjob starts on boot after an image flash.

Note, if 'Keep settings' is selected - see above - a prior crontab may be preserved and operational instead of the 1.3.0 crontab. Regardless, the Version 1.3.0 cronjob is always present and available for reference in file form as `/root/wccf_sensors.cron`.

The current cronjob can be viewed and changed through the Web U/I at menu `System -> Scheduled Tasks`.  Alternatively, when ssh'd into the AP, from the command line use `$ crontab -e` to view and edit these configurations, and `$ crontab -h` for more options.

There are four Sensors in WCCF, 1.3.0: 

- /usr/sbin/wccf_sensor_scan
- /usr/sbin/wccf_sensor_station
- /usr/sbin/wccf_sensor_channel
- /usr/sbin/wccf_sensor_interface

Each must be configured with cmd line arguments that designate (a) a specific interface\*\* and (b) a file output directory.  Note that the output directory (b) should match the input directory configured for the Transmitter running on the AP (see below).  See the existing crontab for examples of invoking sensors.

\*\* Sensor wccf_sensor_interface does not accept an interface; rather, it executes upon all interfaces each invocation.

Output of the sensors is in JSON format.  Specific output-content of each Sensor is documented in the WCCF git respository [wccf/doc/](./) directory.

###iii. Processors###

In WCCF, Processors are apps intended (generally) to accept Sensor output and transform it in some fashion prior to delivery by a Transmitter to a corresponding Receiver.  In the base Version 1.3.0, there is a single Processor that demonstrates this architectural relationship by merely transferring Sensor output to the Transmitter's input directory.  This Processor is named wccf_proc_null.

The config file for wccf_proc_null is file [`/etc/config/wccf_proc_null.cfg`](../utils/wccf_proc_null.cfg.in).  Its content is largely self documenting and is reproduced in its entirety here:

    #
    # Copyright (c) 2017 Applied Broadband, Inc., and
    #                    Cable Television Laboratories, Inc. ("CableLabs")
    #
    # Configuration for OpenWrt WCCF process wccf_proc_null
    #
    # Update these values consistent with installed
    # WCCF Sensors(s)
    #
    
    # These values likely DO NOT require change
    export WCCF_PIDFILE=/tmp/run/wccf_proc_null.pid
    export WCCF_INPUT_PATH=/tmp/wccf/sensor
    export WCCF_OUTPUT_PATH=/tmp/wccf/tx
    export WCCF_LOG_PATH=/root/log

Under default OpenWrt behavior, changes to this config file will survive a reflash with a new image - something to be aware of if config changes are included in the new flash.  You may delete the installed config file prior to reflash to pick up the new config values.

The init script for wccf_proc_null is file [`/etc/init.d/wccf_proc_null`](../utils/wccf_proc_null.in).  It controls the location of logging output as well as illustrates the mapping of the above variables in the config file to specfic cmd line input parameters of the process.

Process wccf_proc_null is configured to start on boot by virtue of the presence of its init script installed in /etc/init.d/.  (In other words, there is no chkconfig utility in OpenWrt.)

###iv. Transmitters###

In WCCF, the Transmitter element is responsible for moving data from the WCCF Agent to a Receiver.  In Version 1.3.0, there is a single Transmitter that is configured to accept output from the above Processor and send it off of the AP to a configured Receiver using REST.  This Transmitter is named wccf_tx_rest.

The config file for wccf_tx_rest is file [`/etc/config/wccf_tx_rest.cfg`](../utils/wccf_tx_rest.cfg.in).  Its content is largely self documenting and is reproduced in its entirety here:

    #
    # Copyright (c) 2017 Applied Broadband, Inc., and
    #                    Cable Television Laboratories, Inc. ("CableLabs")
    #
    # Configuration for OpenWrt WCCF process wccf_tx_rest
    #
    # Update these values consistent with installed
    # WCCF Sensors(s)
    #
    # These values likely DO require change
    export WCCF_RECEIVER_HOST=208.113.130.113
    export WCCF_RECEIVER_PORT=8888
    
    # These values likely DO NOT require change
    export WCCF_PIDFILE=/tmp/run/wccf_tx_rest.pid
    export WCCF_INPUT_PATH=/tmp/wccf/tx
    export WCCF_LOG_PATH=/root/log

Under default OpenWrt behavior, changes to this config file will survive a reflash with a new image - something to be aware of if config changes are included in the new flash.  You may delete the installed config file prior to reflash to pick up the new config values.

The init script for wccf_tx_rest is file [`/etc/init.d/wccf_tx_rest`](../utils/wccf_tx_rest.in).  It controls the location of logging output as well as illustrates the mapping of the above variables in the config file to specific cmd line input parameters of the process.

Process wccf_tx_rest is configured to start on boot by virtue of its init script installed in /etc/init.d/.

###v. Receivers###

The Receiver is installed on a server that will act as the data target for the remote WCCF AP's.

The Receiver is a Python module that consists of 2 files:

 + bottle.py - A standalone opensource web server library (MIT license)
 + wccf_rx_rest.py - The main receiver code

**Basic Receiver Server Requirements**

  + 1 PU 2G RAM minimum (for larger installations, more may be required but for testing, this is sufficient)
  + CentOS 7
  + Python 2.7

One additional Python library, Paste, is required to run this version of the Receiver.  Paste provides well-established multithreading and asynchronous web access through an MIT-licensed Python library.  Paste is required and can be installed using the following pip command (as root):

    $ pip install paste

**Receiver Installation**

  1. As root, copy bottle.py and wccf_rx_rest.py to /usr/sbin (This is recommended but other locations are possible.  These instructions assume /usr/sbin.)
  2. Change permissions on the 2 files.  As root: 
      `chmod 755 /usr/sbin/bottle.py; chmod 755 /usr/sbin/wccf_rx_rest.py`
  3. Install the Python Paste library if not present.  As root: 
      `pip install paste`

**Receiver Logging**

By default, wccf_rx_rest.py logs to CentOS service rsyslog, facility local5.  This is hardwired in the code at this time.

Without any changes, log messages up to INFO will be logged to /var/log/messages.  If you wish to have DEBUG level messaging enabled, and/or desire a separate log for wccf_rx_rest, you will need to add the following line to `/etc/rsyslog.conf` just before the comment as illustrated here:

    # ### begin forwarding rule ###
    local5.*                                                /var/log/wccf.log

A restart of service rsyslog is required to enable this change.

**Running the Receiver**

Help for wccf_rx_rest.py is available with the -h switch:

    $ python2.7 /usr/sbin/wccf_rx_rest.py -h
    Usage: wccf_rx_rest.py [options]

    Options:
      -h, --help            show this help message and exit
      -o OUTPUTPATH, --outputpath=OUTPUTPATH
                            output path for file storage [default: /tmp]
      -p HTTPPORT, --port=HTTPPORT
                            Port for the local HTTP server [default: 8080]
      -H HTTPHOST, --hostname=HTTPHOST
                            Hostname used for local http server (IP or hostname)
                            [default: localhost]
      -D LOGLEVEL, --debug=LOGLEVEL
                            DEBUG level, [ERROR|WARN|INFO|DEBUG] [default: INFO]

To execute wccf_rx_rest.py in the background, run the following command, substituting approriate values for the data directory location, IP address and port numbers:

    $ nohup python2.7 /usr/sbin/wccf_rx_rest.py -o /data/wccf -p 8888 -H 208.113.130.113 &> /dev/null &

The designated output directory will be created (if necessary).

##4. Configuring the OpenWrt Device##

OpenWrt offers a full Web-based U/I for configuration and management.  While this U/I is only optionally present on any OpenWrt device, the build instructions in [wccf/doc/HOWTO_build_flashable_image.md](./HOWTO_build_flashable_image.md) include the steps required to insure the Web U/I's presence.

WCCF Version 1.3.0 Apps (see Section 3) assume certain settings within the Access Point.  Use of the Web U/I is recommended to set or change these and other device settings.

###i. Scheduled Tasks (crontab)###

The Sensors covered above (Section 3) are triggered periodically to capture Wi-Fi information via the familiar crontab.  

In the U/I, menu `System -> Scheduled Tasks` will show the present crontab for user root (as installed by the WCCF image flash).  The crontab for user root is preserved across reflashes.

###ii. WAN Interface Config (bridging)###

All of the sensors utilize the Interface BR-WAN to access a MAC address for their data collection (which appears in the Sensor output filenames for identification).  This Interface is created as a consequence of enabling bridging on the WAN.  

In the U/I, menu `System -> Interfaces` presents an Interface Overview page.  From there, find WAN under the column labeled Network, and click the Edit button to its right.  On the next page, Interfaces - WAN, under Common Configuration, select the tab labeled `Physical Settings`.  Check the box labeled Bridge interfaces, then click `Save & Apply` at the bottom of the page.  Once this has been done, the Network Interfaces - Interface Overview page shows "br-wan" under the WAN network.  When a reflash is done with "Keep settings" selected, the BR-WAN Interface is preserved.

###iii. Firewall###

If the Firewall rules presented above (Section 2) have been added, these rules can be observed in the Wed U/I at `Network -> Firewall`, tab `Traffic Rules`.  They can be added here, also, if not present.  When a reflash is done with "Keep settings" selected, the Firewall rules will be preserved.

###iv. Build Image Management###

Over time, OpenWrt images may proliferate, representing various options, etc.  This is a file management issue on the build machine.  However, on the Access Point itself, it's useful to be able to identify the origin of the flashed image.  In the Web U/I, under `Status -> System Log`, the first line shown includes the build username@hostname along with a build timestamp.  Alternatively, you may type ```logread``` at the command line in an ssh session to view syslog.

More detail on OpenWrt logging is available here: [https://wiki.openwrt.org/doc/howto/log.essentials](https://wiki.openwrt.org/doc/howto/log.essentials).

