# Wireless Common Collection Framework - WCCF

# HOWTO\_update\_ap\_config

### Version 1.4.0

`Copyright (c) 2017 Applied Broadband, Inc., and`
`                   Cable Television Laboratories, Inc. ("CableLabs")`

This HOWTO assumes you have completed all the previous steps to create a WCCF-enabled image and flash an AP device (see [wccf/doc/HOWTO_start_here.md](./HOWTO_start_here.md)).

>NOTE: Factory-image TP-Link AP devices are set up on the subnet 192.168.0.1 whereas OpenWrt images utilize 192.168.1.1.  Therefore, take care when using browser bookmarks or command-line values to successfully reach the AP.

## Background on WCCF-Enabled OpenWrt AP Configuration

By default, the OpenWrt build configuration has the following characteristics:

- There is no root password for SSH or HTTP access to the device (which is convenient but a security risk).

- Wireless service is completely disabled for both radios.  In addtion, once it is enabled - 

   - no wireless passwords exist
   - wireless encryption is still disabled
   - the SSID defaults to OpenWrt for both radios
   - the 5GHz (802.11ac) radio selection may be incorrect on the TP-LINK

- The WAN I/F is not set to 'Bridged', meaning interface BR-WAN does not exist (but is required for proper sensor operation).  The WAN I/F does default to DHCP-acquired IP address (if cabled).

- The LAN I/F (wired ports) does not provide DHCP sevice to connected stations so, to access, computers must be previously assigned a static address in the 192.168.1.x subnet (recommended: 192.168.1.2).

- The Firewall configuration disallows TCP traffic over the WAN interface on ports 80 (HTTP) and 22 (SSH).  This means access *must* be through the wired LAN ethernet ports since wireless is disabled.

Each of the above issues can be addressed by accessing the AP through its Web U/I management interface, or by ssh-ing into the AP and editing config files and setting up a password by hand.  The procedures documented below, using WCCF-included config files, will overcome each of these issues in semi-automated fashion (using scp).  Note that, in any case, a computer must be connected to the device to load the WCCF OpenWrt image file.  If the AP is running a factory image at load time, DHCP is likely available.

## Connecting to the AP

At this stage, the WCCF-enabled OpenWrt image has been flashed to the AP, either using the Web U/I or using scp and ssh.  The following steps assume command-line access to the AP with ssh and scp.  The LAN ethernet ports give access to the AP at 192.168.1.1 by default when the connected station has a pre-assigned static IP address in the 192.168.1.x subnet.  For this reason, individual AP units must be processed 'single file'. (Note that connection of the WAN port - to the Internet - is not assumed, and likely DNS service is not available ... meaning, for example, domain names such as yahoo.com will not be recognizable, and date/time are incorrect.)

### Setting a root Password

OpenWrt allows the root password to be set from the command line but of course it's always a multi-step process.  A provided WCCF config file, `shadow`, can be copied onto the AP (as file `/etc/shadow`) to set the password.  The provided shadow file includes the key to set root password to `Wccf.Rt.Pw`.  To set a different password, use the AP cmd line `passwd` and retreive the new copy of `/etc/shadow`.

There is a single shadow file that is used for both AP models (Netgear and TP-LINK).

### Enabling and Configuring the Wireless Router

Wireless is configured in file `/etc/config/wireless`.  The WCCF config file, wireless, enables DHCP for connecting stations and sets the following values:

- SSIDs: WccfOpenWrt-2.4 and WccfOpenWrt-5
- Encryption Type: WPA2-PSK (psk2)
- Wireless Password: Wccf.Ap.Pw [Note: similar to, but different from the root pw]
- Other settings per AP model

There is a separate wireless config file for each AP type - Netgear WNDR3800 and TP-LINK Atcher C7.

### Enabling Bridged WAN - BR-WAN - Interface

The WCCF file `network` enables the WAN bridge setting required for proper sensor operation.

### Enabling HTTP and SCP over WAN Interface

The WCCF file `firewall` enables TCP over ports 22 and 80 on the WAN interface.

## Moving Provided Files to AP

1. Move the provided files, choosing netgear or tp-link (here [wccf/utils/config_files](../utils/config_files)) to a computer with access to the AP
2. Verify file contents and make any desired changes (eg, wireless password or SSID)
3. Set the NIC for the computer that received the files in step 1 to have static IP 192.168.1.2
4. Check that you don't have an old host key in ~/.ssh/known_hosts for 192.168.1.1 (if so, simply delete it)
5. Connect, wired as described above, to the AP
6. Enter the following commands:

        $ cd <location that provided files of step 1 were placed>
        $ ./copy_configs.sh
        $ ssh root@192.168.1.1 # Note: root password is now Wccf.Rt.Pw
        root@openwrt:~# reboot

The device should now reboot and come up with the root password still set, wireless enabled, and the WAN interface enabled for scp and http.  If you made a change to the /etc/shadow file prior to uploading it, your ssh login will be expecting that chosen value.

