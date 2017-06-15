# WCCF Admin Guide
## Version 1.4.1.  June 15, 2017

Copyright (c) 2017 Applied Broadband, Inc., and Cable Television Laboratories, Inc. ("CableLabs")

## Contents

1. Overview
2. Installing WCCF Version 1.4.x
3. Configuring WCCF Version 1.4.x Apps
    - i. Space Management
    - ii. Sensors
    - iii. Processors
    - iv. Transmitters
    - v. Receivers
4. Configuring the OpenWrt Device
    - i. Scheduled Tasks (crontab)
    - ii. WAN Interface Config (dhcp, bridging)
    - iii. Firewall
    - iv. Build Image Management

5. Setting Up rsync For Config File Management
    - i. OpenWrt rsync and ssh
    - ii. Dropbear public-private Key Pair Creation and Installation
    - iii. Remote Server Endpoint Preparation
    - iv. The WCCF rsync File Transfer Utility

6. The WCCF Updater Capability
    - i. The Main Pieces
    - ii. File Details
    - iii. The Update Process Overview

## 1. Overview

Wi-Fi Common Collection Framework (WCCF), Version 1.4.x, targets OpenWrt-based Access Point (AP) hardware (https://openwrt.org/) running OpenWrt branch master (aka Designated Driver).  WCCF Version 1.4.x has focused on supporting the following access point devices:

+ Netgear N600 Model WNDR3800 dual band with 2.4GHz 802.11b/g/n and 5GHz 802.11a/n,
+ TP-Link AC1750 Model Archer C7 dual band with 2.4GHz 802.11b/g/n and 5GHz 802.11a/n/ac capability.

This document covers the details of configuring and operating WCCF services within an OpenWrt Access Point image built with WCCF.  This document also addresses the configuration of a "Receiver" network element to which the WCCF-enabled Access Point will send its collected data.

The WCCF Version 1.4.x release includes fully-functional system images ready to flash onto the TP-Link device (see above) for demonstration, testing, and production AP operation.  See Section 4, Configuring the OpenWrt Device, for details on AP configuration with these images.

Other technical documentation provided in release 1.4.x covers the following topics:

+ WCCF HOWTO Docs Overview ([wccf/doc/HOWTO_start_here.md](./HOWTO_start_here.md))
+ WCCF/OpenWrt Build Machine ([wccf/doc/HOWTO_build_machine.md](./HOWTO_build_machine.md))
+ Building WCCF ([wccf/doc/HOWTO_build_wccf.md](./HOWTO_build_wccf.md))
+ Building WCCF into OpenWrt ([wccf/doc/HOWTO_build_flashable_image.md](./HOWTO_build_flashable_image.md))
+ Samples of each Sensor's output content and format in the reponsitory [wccf/doc/](./).

## 2. Installing WCCF Version 1.4.x

The image files included with release 1.4.x are TP-LINK OpenWrt "sysupgrade" and "factory" format images at  [wccf/qa/images/1.4.1](../qa/images/1.4.1).  The sysupgrade format is suitable to upgrade ("flash") an existing OpenWrt device.  It is not suitable for use as a "first installation" of OpenWrt onto a factory-image based access point.  The "factory" image is such a "first installation" image. Either can be built following the steps detailed in the HOWTO documents above.

To install either provided image, one can either interact with the target device using its Web-based OpenWrt U/I, or one can scp the image onto the device, then ssh to the device and force the flash using the command line.

### Using the Web-based U/I to Reflash the Device
Access the Access Point's OpenWrt Web U/I from any web browser.  You will need the root-user password for the device to log on.

Under the System menu, choose item "Backup / Flash Firmware" and then scroll down to "Flash new firmware image."

Decide whether to check the "Keep settings" box (**recommended**).  See "Reflashing using the Command Line", below, for tips on how to see exactly what will be preserved in a reflash if you choose "Keep settings", and how to interactively alter the specific behavior of "Keep settings" if using the command-line technique.

If you do not select to keep settings, various AP values such as IP addresses and wireless settings likely will be lost (effectively like doing a firmware reset on any AP).  Additionally, the wireless radios themselves may be disabled when settings are lost.  Finally, the root password for ssh access may be lost, requiring you to reset the root password, for web and ssh access.  (*Note that these are all basic OpenWrt behaviors, not WCCF behaviors.  Consequently, it is recommended that you initially work locally, as a Station attached to the AP - wireless or wired - until such behaviors become familiar.*)

****
WARNING: Resetting the AP, especially if *not* saving settings, may cause loss of network access depending upon how you have connected to the device.  For example, the WAN port may revert to a new DHCP-acquired address from a previously defined static address, or may revertto a prior IP address.  In such cases, your only alternative to reconnect may be directly through a wired LAN port on the AP (which may require physical access to the device).
****

Continuing using the Web U/I, using the Choose File browser button, select the desired image file (in this release it will be named openwrt-ar71xx-generic-\<profile\>-squashfs-sysupgrade.bin, where \<profile\> is wndr3800 or archer-c7-v2, but NOT archer-c7-v1), then proceed with the re-flash by clicking the "Flash Image..." button, and finally, the "Proceed" button.  

Typically, the flash and reboot will take no more than 3-4 minutes.

### Reflashing using the Command Line

Rather than using the OpenWrt Web U/I, the new image can be copied (scp) to the device and flashed using the command line.  SSH access to the AP is required for the second step in this process.  Note that to connect to the AP over its WAN port (vs a LAN port) the SSH process (called dropbear in OpenWrt) must be configured to allow SSH through that WAN interface, and the firewall process (called firewall) must have a rule to allow TCP through the WAN interface, port 22.  You may also wish to allow port 80 access (http) through the WAN interface.

For these changes to be made, access first must take place over a LAN port - either through SSH or via the Web U/I.  The following steps describe a command-line approach.

In file `/etc/config/firewall`, enable ssh access over WAN, and (*optionally*) http over WAN.  Add these sections to the end of the file:

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

In file `/etc/config/dropbear`, include only one of the two option statements below to designate which interface allows SSH.  For support on both interfaces, **remove both** statements from the config (which would correspond to 'unspecified' in the Web U/I):

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


## 3. Configuring WCCF Version 1.4.x Apps

### i. Space Management
Storage space generally is limited on OpenWrt devices.  WCCF apps use some storage space on the AP.  Generally, they configure space under /tmp for files only until they are transferred to a Receiver at another address.

App logging is initially configured to go to /root/log.  Although this location may be on a smaller partion than /tmp, content is preserved across reboots which is a benefit.

See the following sections for details on configuring file locations.

### ii. Sensors

In WCCF, Version 1.x, Sensors exist that obtain Wi-Fi information and store it in files for subsequent delivery through a Transmitter to a remote Receiver.  In Version 1.x, crontabs are used to trigger the Sensors periodically (initially, at 5-minute intervals).  The WCCF Version 1.x build/install package installs a cronjob for user root with pre-configured sensor entries.  This cronjob starts on boot after an image flash.

Note, WCCF 1.4.1 includes a ConTab update.  If 'Keep settings' is selected - see above - a prior OpenWrt crontab may be preserved and operational instead of the 1.4.1 crontab.  Regardless, the most current cronjob is always copied onto the AP and so is present and available for reference in file form as `/root/wccf_sensors.cron`.  If upgrading to 1.4.1 from a prior WCCF version, you MUST change the new crontab with the following command (or use the GUI Console to edit it):

        $ crontab /root/wccf_sensors.cron

The current cronjob can be viewed and changed through the Web U/I at menu `System -> Scheduled Tasks`.  Alternatively, when ssh'd into the AP, from the command line use `$ crontab -e` to view and edit these configurations, and `$ crontab -h` for more options.

There are five Sensors in WCCF, 1.4.x: 

- /usr/sbin/wccf_sensor_scan
- /usr/sbin/wccf_sensor_station
- /usr/sbin/wccf_sensor_phycapa
- /usr/sbin/wccf_sensor_channel
- /usr/sbin/wccf_sensor_interface

Each must be configured with cmd line arguments that designate (a) a specific interface\*\* and (b) a file output directory.  Note that the output directory (b) should match the input directory configured for the Transmitter running on the AP (see below).  See the existing crontab for examples of invoking sensors.

\*\* Sensor wccf_sensor_interface does not accept an interface; rather, it executes upon all interfaces each invocation.

Output of the sensors is in JSON format.  Specific output-content of each Sensor is documented in the WCCF git respository [wccf/doc/](./) directory.

### iii. Processors

In WCCF, Processors are apps intended (generally) to accept Sensor output and transform it in some fashion prior to delivery by a Transmitter to a corresponding Receiver.  In Version 1.x, there is a single Processor that demonstrates this architectural relationship by merely transferring Sensor output to the Transmitter's input directory.  This Processor is named wccf_proc_null.

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

### iv. Transmitters

In WCCF, the Transmitter element is responsible for moving data from the WCCF Agent to a Receiver.  In Version 1.x, there is a single Transmitter that is configured to accept output from the above Processor and send it off of the AP to a configured Receiver using REST.  This Transmitter is named wccf_tx_rest.

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

### v. Receivers

The Receiver is installed on a server that will act as the data target for the operating WCCF AP transmitter(s).

The provided WCCF Receiver is a Python module that consists of 2 files:

 + bottle.py - A standalone opensource web server library (MIT license)
 + wccf_rx_rest.py - The main receiver code implementing a REST API to receive transmitted json files

**Receiver Server Requirements**

There are no base requirements for a WCCF Receiver other than the capability to run Python 2.7 and to handle generated traffic.  As configured by default, each WCCF-enabled AP will generate approximately 20-25K bytes of output once every five minutes.  (The total byte-count received per AP per cycle is *unrelated* to the amount of broadband traffic handled locally by each AP.)

A typical Receiver host profile may be:

  + 1 PU 2G RAM minimum (for larger installations, more may be required but for testing, this is sufficient)
  + CentOS 7 or equivalent Linux Distro (eg, Unbuntu - note: CentOS 7 used for testing)
  + Python 2.7

One additional Python library, Paste, is required to run the current version of the Receiver.  Paste provides well-established multithreading and asynchronous web access through an MIT-licensed Python library.  Paste is required and can be installed using the following pip command (as root):

    $ pip install paste

If pip is not installed, use this command:

    $ yum install -y python-pip

**Receiver Installation**

  1. As root, copy bottle.py and wccf_rx_rest.py to /usr/sbin (This is recommended but other locations are possible.  These instructions assume /usr/sbin.)
  2. Change permissions on the 2 files.  As root: 
      `chmod 755 /usr/sbin/bottle.py; chmod 755 /usr/sbin/wccf_rx_rest.py`
  3. Install the Python Paste library if not present.  As root: 
      `pip install paste`

**Receiver Logging**

By default, wccf_rx_rest.py logs to CentOS service rsyslog, facility local5.  This is hardwired in the code at this time.  Some useful logging output may go to stdout so the startup command redirects this to a file also.

Without any changes, log messages up to INFO will be logged to /var/log/messages.  If you wish to have DEBUG level messaging enabled, and/or desire a separate log for wccf_rx_rest, you will need to add the following line to `/etc/rsyslog.conf` just before the comment as illustrated here:

    # ### begin forwarding rule ###
    local5.*                                                /var/log/wccf.log

The rsyslog service on Ubuntu differs from CentOS so configuration may require additional steps.  The following ownership change was found to be necessary in cursory testing:

    $ chown syslog:adm /var/log/wccf.log

A restart of service rsyslog is required to effect the above changes.

    $ service rsyslog restart

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

To execute wccf_rx_rest.py in the background, run the following command, substituting your approriate values for the data directory location, the Receiver's IP address and port numbers:

    $ nohup python2.7 /usr/sbin/wccf_rx_rest.py -o /data/wccf -p 8888 -H 10.10.10.101 &> $HOME/wccf_rcvr.log &

The designated output directory in the above startup command will be created (if necessary) provided that the user under which the Receiver is run has sufficient permissions to do so.  The Receiver will create, under that directory, a subdir for each AP named with the AP's MAC address.  

If files appear to be failing to reach the Receiver, review the sensor crontab settings on your AP (more detail in section \#4, below), insure that the process wccf_tx_rest is running and configured as required on your AP, and test the accessibility of the Receiver service with the following command:

    $ telnet 10.10.10.101 8888

    Trying 10.10.10.191...
    Connected to 10.10.10.101.

If you do not see the telnet session connect as above (rather, you see for example Connection refused, or, No route to host), then check all normal networking issues such as routes, firewalls, port numbers, etc.  Also verify that the logging output of the Receiver includes the output:

    Listening on http://10.10.10.101:8888/


## 4. Configuring the OpenWrt Device

OpenWrt offers a full Web-based U/I for configuration and management.  While this U/I is only optionally present on any OpenWrt device, the build instructions in [wccf/doc/HOWTO_build_flashable_image.md](./HOWTO_build_flashable_image.md) include the steps required to insure the Web U/I's presence.

WCCF Version 1.x Apps (see Section 3) assume certain settings within the Access Point.  Use of the Web U/I is recommended to set or change these and other device settings.

### i. Scheduled Tasks (crontab)

The Sensors covered above (Section 3) are triggered periodically to capture Wi-Fi information via the familiar crontab. 

In 1.4.x, the rsync config synchronizer is also triggered periodically by crontab.

In the Web U/I, menu `System -> Scheduled Tasks` will show the present crontab for user root (as installed by the WCCF image flash).  The crontab for user root is preserved across reflashes.

### ii. WAN Interface Config (dhcp, bridging)

The AP WAN interface General Setup should remain set to `DHCP client` to receive its IP address from an external DHCP server (eg, the Cable Modem).  This also provides DNS on the AP to resolve such names as NTP time servers.  If the WAN is changed to `Static address`, DNS servers must also be provided on the page.

All of the WCCF sensors utilize the Interface `BR-WAN` to access a MAC address for their data collection (which appears in the Sensor output filenames for identification).  The `BR-WAN` Interface is created as a consequence of enabling bridging on the WAN as follows:  

In the Web U/I, menu `Network -> Interfaces` presents an Interface Overview page.  From there, find WAN under the column labeled Network, and click the Edit button to the far right.  On the next page, Interfaces - WAN, under Common Configuration, select the tab labeled `Physical Settings`.  Check the box labeled `Bridge interfaces` (ie, see that the box is checked).  Then verify that the Interface box labelled `Ethernet Adapter: "eth1"` is the only selected box.  Finally, click `Save & Apply` at the bottom of the page.  Once this has been done, the Network Interfaces - Interface Overview page shows the text "br-wan" the WAN network graphic.  When a reflash is done with "Keep settings" selected, the BR-WAN Interface is preserved.

### iii. Firewall

If the Firewall rules presented above (Section 2) have been added, these rules can be observed in the Wed U/I at `Network -> Firewall`, tab `Traffic Rules`.  They can be added here, also, if not present.  When a reflash is done with "Keep settings" selected, the Firewall rules will be preserved.

### iv. Build Image Management

Over time, OpenWrt images may proliferate, representing various options, etc.  This is a file management issue on the build machine.  However, on the Access Point itself, it's useful to be able to identify the origin of the flashed image.  In the Web U/I, under `Status -> System Log`, the first line shown includes the build username@hostname along with a build timestamp.  Alternatively, you may type ```logread``` at the command line in an ssh session to view syslog.

More detail on OpenWrt logging is available here: [https://wiki.openwrt.org/doc/howto/log.essentials](https://wiki.openwrt.org/doc/howto/log.essentials).

## 5. Setting Up rsync For AP Config File Management

Important Note: Due to this following issue, rsync will be used without compression in release 1.4.x.

https://icesquare.com/wordpress/this-rsync-lacks-old-style-compress-due-to-its-external-zlib-try-zz/

To support WCCF config file management, there are the following steps:

i. The rsync and ssh utilities must be present in the OpenWrt image.  
ii. A Dropbear public-private key pair must be created and installed properly on the endpoints.  
iii. A remote server endpoint must be identified and prepared to interact with the WCCF AP devices.  
iv. A file transfer utility must be present and configured on the AP and configured to run via crontab at scheduled intervals.  

### i. OpenWrt rsync and ssh

These utilities were included in the `make menuconfig` step in [wccf/doc/HOWTO_build_flashable_image.md](./HOWTO_build_flashable_image.md).

### ii. Dropbear public-private Key Pair Creation and Installation

The WCCF repository includes a prepared key pair for this purpose.  The WCCF OpenWrt image locates the private key at `/etc/config/wccf_openwrt_privkey`.  The public key is available to install as shown below on the remote server in `/home/<user>/.ssh/authorized_keys` where `<user>` matches the name set in the file transfer utility config.

For background only, we show the following OpenWrt commands (adapted from here: [https://yorkspace.wordpress.com/2009/04/08/using-public-keys-with-dropbear-ssh-client/](https://yorkspace.wordpress.com/2009/04/08/using-public-keys-with-dropbear-ssh-client/])) to create a key pair on an OpenWrt device:

    dropbearkey -t rsa -f /etc/config/wccf_openwrt_privkey
    dropbearkey -y -f /etc/config/wccf_openwrt_privkey | grep "^ssh-rsa " >> /etc/config/wccf_openwrt_pubkey
    cat /etc/config/wccf_openwrt_pubkey > /tmp/authorized_keys
    
As noted above, these keys are already prepared and included in the WCCF repository and in the WCCF OpenWrt image.

### iii. Remote Server Endpoint Preparation

The resulting tmp file from step ii `/tmp/authorized_keys` would then be transferred to the remote server and concatenated onto the end of the rsync user's file `~/.ssh/authorized_keys` as follows:

    cat /tmp/authorized_keys >> ~/.ssh/authorized_keys

Here is an illustration using `wccfadm@10.10.10.21`.  The choice of server will be driven by its accessibility from the AP and vailability of resources on the server.

Once the keys are in place (step ii, above), the following two commands entered at the AP command line should work (be sure to use your remote server username and ip address to validate):

    # should gain access with no password challenge
    ssh -i /etc/config/wccf_openwrt_privkey
    
    # file transfer can be verified on the remote server
    rsync -av -e "ssh -y -i /etc/config/wccf_openwrt_privkey" \
        /etc/config/wccf_proc_null.cfg \
        wccfadm@10.10.10.21:wccf_rsync_incoming/wccf_proc_null.cfg

### iv. The WCCF rsync File Transfer Utility

WCCF includes a two-way configuration backup/updating capability to facilitate some types of in-field updates on a WCCF AP.  Specifically, in the first step, files in AP's directory `/etc/config/` are copied to a remote location *but skipping files that are newer on the receiver*.  Then, in a second step, files that are newer on the remote location are copied back to the AP.  If the process detects new files arriving on the AP, a reboot is triggered at the end of the second copy step.

This capability uses rsync with ssh and public and private keys and ssh settings that allow execution with no human intervention.  WCCF provides a public/private key pair for this purpose, though another pair could be substituted prior to building the flashable image.

The WCCF image build places the private key on the AP and points to it from a config file (the key is not stored into `/root/.ssh/` on the AP).  The remote (receiver) endpoint *must be configured* with the provided public key for the sync capability to work.  To do this, cat the public key into the receiver's known_hosts file (by default, file `/home/wccfadm/.ssh/known_hosts`) in the normal fashion.

The rsync capability is triggered by an entry in the WCCF crontab (see [wccf/utils/wccf_sensors.cron.in](../utils/wccf_sensors.cron.in) which controls the timing and periodicity of sync (which also can be adjusted prior to image build).

The WCCF-enabled OpenWrt image includes this utility here: `/usr/sbin/wccf_config_sync.sh`.  Its configuration is here: `/etc/config/wccf_config_sync.cfg`.  The configuration file may be updated with the desired username and remote IP address within the git repo before the WCCF-OpenWrt image is built.  The git repo includes the config here: [wccf/utils/wccf_config_sync.cfg.in](../utils/wccf_config_sync.cfg.in).

Also in the wccf_config_sync.cfg file are two more values:

- the destination directory on the remote sync host (by default `/home/wccfadm/config_sync/`)
- the location, on the WCCF OpenWrt AP, of the private key to be used in the scp transfer

Note that the destination directory on the remote server must be created in advance and exist for the sync to work.  Subdirectories are then created beneath that location automatically as part of the sync process.  In particular, files from each AP are collected under that AP's MAC address.  The sync process starts by moving newer files *from* the AP to the remote server and after that, moves newer files on the remote *back to* the AP.



## 6. The WCCF Updater Capability

Starting with version 1.4.0, WCCF includes a simple update capability to add or change content on an AP in the field based upon its MAC address.  The AP MAC address is used in the rsync config backup/updating capability (see #5).  The Updater capability is enabled by default and its scripts operate routinely but are, by default, defined as no-op behavior.  It is this *always on* character that allows desired updates to be detected and effected at any point in time.  By default, the Updater is triggered four minutes after every rsync cycle.

Operations tested with the Updater capability include operations such as changing the Crontab settings, and actually downloading and installing a new WCCF-OpenWrt image and automatically flashing it on the AP in a hands-free manner.  The pieces for these tests appear in [wccf/qa/updater](../qa/updater).  In particular, see the [README](../qa/updater/README) there.

### i. The Main Pieces

Pieces of the Updater capability include:

(1) the crontab entry that triggers the update script,   
(2) the update script itself that's triggered by the crontab,   
(3) the worker script invoked by the update script (#2), and   
(4) any related files referenced by the worker script.

### ii. File Details

The default WCCF crontab runs the update script.  The update script (/usr/sbin/wccf_util_update.sh) runs the provided no-op worker script (/etc/config/wccf_util_update_worker.sh). The worker script *must* reside in /etc/config for the rsync download and automatic execution to work properly.  Some test files for different update tasks appear in [wccf/qa/updater](../qa/updater).

The default worker script is a NO-OP, but contains important content.  Specifically, the worker script contains a function that is always available to restore its behavior to NO-OP.  **ANY NEW BEHAVIOR INTRODUCED INTO THE WORKER MUST ALWAYS END WITH A CALL TO ITS INTERNAL FUNCTION TO RESTORE NO-OP BEHAVIOR.**  This makes the update behavior a once-and-done event (so it's not repeated accidentally with whatever impact that may have on the AP operation).

See file [wccf/qa/updater/wccf_util_update_worker.sh.REFLASH](../qa/updater/wccf_util_update_worker.sh.REFLASH) as an example - in particular, the content *after* line 36.

### iii. The Update Process Overview

To effect one-time behaviors (such as a crontab change or a reflash as illustrated in the files located [here](../qa/updater)), the files are copied to a temp location and modified as desired, renamed as appropriate, and moved to the remote (server-side) endpoint of the wccf_config_sync facility (see Section #5, above).  This remote endpoint is the location to and from which each AP syncs config files.  Note that, when first syncing, each AP sets up its own subdirectory on the remote - so in order to update ten APs in the field, the collection of update files (see i. 1 - 4) would have to be copied into each of the ten subdirectories corresponding to those target APs.  Recall that those directories are named with the MAC address of each AP.  Hence, the target APs' MAC addresses must be known.

The above process final file movement can be implemented with its own automation to reduce errors and/or implement more periodic changes over time.

With the update files in place, the standing sync process then moves files from and to the AP local directory /etc/config.  These update-related files will be delivered to the AP in the next sync cycle.  

> It should be noted that a minute possibility exists that the set of files may transfer into the remote rsync location *during* an AP sync cycle.  To remediate the impact of such, the last file moved into the remote sync directory should be the worker script.  Also, the file should be *moved* vs copied to the destination since a Unix mv is atomic.

Shortly after the rsync completes (4 minutes later in the default crontab file), the update will trigger and take place.

