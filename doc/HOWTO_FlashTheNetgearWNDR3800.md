#Uploading and Flashing a SysUpgrade OpenWrt Build Product to the WNDR3800#
    
`Copyright (c) 2017 Applied Broadband, Inc., and`
`                   Cable Television Laboratories, Inc. ("CableLabs")`

This example assumes the build machine has direct access to the Netgear WNDR3800 Access Point.  This is not a requirement - the .bin can be scp'd anywhere.  Also, simply for clarity, this example upgrades OpenWrt Release Chaos Calmer to Release Designated Driver.  The same process is used to introduce any updated image to the AP.

This example assumes the build machine user has env var OPENWRT_BUILD_ROOT defined and a local Git repo there, which has been built according to the HOWTO docs in wccf/doc.
    
This example assumes the Netgear WNDR3800 is addressable at 10.10.10.129.
    
On Build Machine:
    
    $ cd ${OPENWRT_BUILD_ROOT}/openwrt/bin/ar71xx
    $ scp openwrt-ar71xx-generic-wndr3800-squashfs-sysupgrade.bin root@10.10.10.129:
    root@192.168.1.1's password:
    openwrt-ar71xx-generic-wndr3800-squashfs-sysupgrade.bin            100% 4864KB 148.6KB/s   00:32
        
    
##SSH into Netgear on the network (at 10.10.10.129)##
    
    $ ssh root@10.10.10.129
    root@10.10.10.129's password:
    
##Verify Netgear originally running OpenWrt Chaos Calmer (or any older version)##
    
    BusyBox v1.23.2 (2017-01-12 09:01:04 MST) built-in shell (ash)
    
      _______                     ________        __
     |       |.-----.-----.-----.|  |  |  |.----.|  |_
     |   -   ||  _  |  -__|     ||  |  |  ||   _||   _|
     |_______||   __|_____|__|__||________||__|  |____|
              |__| W I R E L E S S   F R E E D O M
     -----------------------------------------------------
     CHAOS CALMER (Chaos Calmer, r49389)
     -----------------------------------------------------
      * 1 1/2 oz Gin            Shake with a glassful
      * 1/4 oz Triple Sec       of broken ice and pour
      * 3/4 oz Lime Juice       unstrained into a goblet.
      * 1 1/2 oz Orange Juice
      * 1 tsp. Grenadine Syrup
     -----------------------------------------------------
    

##Verify current OpenWrt image date/time (placed during prior image flash)##
    
    root@OpenWrt:~# ls -lart
    drwxr-xr-x    1 root     root             0 Jan 23 17:16 ..
    -rw-r--r--    1 root     root       4456452 Jan 24 16:38 openwrt-ar71xx-generic-wndr3800-squashfs-sysupgrade.bin
    
**New OpenWrt image is then copied onto the Netgear *from build machine* (see above).**

    
##Verify updated OpenWrt image date/time##
    
    root@OpenWrt:~# ls -lart
    drwxr-xr-x    1 root     root             0 Jan 23 17:16 ..
    -rw-r--r--    1 root     root       4456452 Jan 25 09:24 openwrt-ar71xx-generic-wndr3800-squashfs-sysupgrade.bin
    

##On Netgear AP, force new image flash from cmd line (vs GUI)##

    root@OpenWrt:~# sysupgrade ./openwrt-ar71xx-generic-wndr3800-squashfs-sysupgrade.bin
    Saving config files...
    killall: watchdog: no process killed
    Sending TERM to remaining processes ... uhttpd odhcp6c ntpd dnsmasq ubusd askfirst logd rpcd netifd odhcpd crond
    Sending KILL to remaining processes ... askfirst
    Switching to ramdisk...
    Performing system upgrade...
    Unlocking firmware ...
    
    Writing from <stdin> to firmware ...
    Appending jffs2 data from /tmp/sysupgrade.tgz to firmware
    Upgrade completed
    Rebooting system...

**Important Note:** Verify that you see the above message "Upgrade copmleted" as part of the AP output.  If the upgrade process is unsuccessful, the AP may boot back into the old image and not reflect the desired upgrade.

##Reboot complete; restore SSH connection to Netgear AP over network##

Verify version is updated to Designated Driver.
    
    Connection reset by 10.10.10.129 port 22
    
    $ ssh root@10.10.10.129
    root@10.10.10.129's password:
    
    
    BusyBox v1.24.2 () built-in shell (ash)
    
      _______                     ________        __
     |       |.-----.-----.-----.|  |  |  |.----.|  |_
     |   -   ||  _  |  -__|     ||  |  |  ||   _||   _|
     |_______||   __|_____|__|__||________||__|  |____|
              |__| W I R E L E S S   F R E E D O M
     -----------------------------------------------------
     DESIGNATED DRIVER (Bleeding Edge, 50082)
     -----------------------------------------------------
      * 2 oz. Orange Juice         Combine all juices in a
      * 2 oz. Pineapple Juice      tall glass filled with
      * 2 oz. Grapefruit Juice     ice, stir well.
      * 2 oz. Cranberry Juice
     -----------------------------------------------------
    root@OpenWrt:~#
    
