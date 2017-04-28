# Wireless Common Collection Framework - WCCF

# HOWTO\_build\_flashable\_image

`Copyright (c) 2017 Applied Broadband, Inc., and`
`                   Cable Television Laboratories, Inc. ("CableLabs")`

### Version 1.4.0

This HOWTO assumes you have previously set up a reference build machine (see [wccf/doc/HOWTO_build_machine.md](./HOWTO_build_machine.md)), and have installed, built and tested OpenWrt locally including both creating and flashing an AP image (see [wccf/doc/HOWTO_start_here.md](./HOWTO_start_here.md)). It also assumes you have completed the steps documented in [wccf/doc/HOWTO_build_wccf.md](./HOWTO_build_wccf.md) which documents how to prepare the WCCF build products to integrate into an OpenWrt Access Point binary image.

The following steps assume that WCCF and OpenWrt are installed and built in parallel directory trees under user wccf.  Make appropriate adjustments for your situation.

		$ mkdir ~/openwrt-source
		$ cd ~/openwrt-source
		$ git clone git://github.com/openwrt/openwrt.git
		$ cd ~/openwrt-source/openwrt

> See utility scripts here [wccf/qa/utilities/build_*](../qa/utilities) that automate these steps (and others).

Copy WCCF pieces (see [wccf/doc/HOWTO_build_wccf.md](./HOWTO_build_wccf.md)) into existing OpenWrt build environment:

		$ mkdir -p ~/openwrt-source/openwrt/dl
		$ cp ~/wccf-source/wccf/wccf-1.4.0.tar.gz ~/openwrt-source/openwrt/dl/
		$ mkdir -p ~/openwrt-source/openwrt/package/network/wccf
		$ cp ~/wccf-source/wccf/targets/openwrt/Makefile.template \
		   ~/openwrt-source/openwrt/package/network/wccf/Makefile
		$ cp -r ~/wccf-source/wccf/targets/openwrt/patches \
		   ~/openwrt-source/openwrt/package/network/wccf/

Notes:

1) The last copy (cp) cmd above includes the -r cmd line switch to move an entire subdirectory.

2) If the above steps are omitted (or done improperly) the choice 'wccf' will be unfound in the step below labeled "Select Network Components".

> See utility scripts here [wccf/qa/utilities/build_*](../qa/utilities) that automate these steps (and others).

Continue with OpenWrt build steps:

		$ ./scripts/feeds update -a
		$ ./scripts/feeds install -a

		$ ./scripts/feeds update packages luci
		$ ./scripts/feeds install -a -p luci
   
		$ make defconfig
		$ make prereq
		$ make menuconfig

The last build target above (menuconfig) causes a character-based GUI to appear in which you will select all necessary components for the new AP flash image.  In each case (where possible) select the indicated item TWICE (to cause its state to go from blank thru M to \*).

### Select the Target System

	Select (X) Atheros AR7xxx/AR9xxx


### Select the Subtarget System

	For our purposes, select (X) Generic.


### Select the Target Profile

	For purposes here, 
	- SELECT EITHER 
	(X) NETGEAR WNDR3700/WNDR3800/WNDRMAC
	- OR -
	(X) TP-LINK Archer C5/C7


### Select the Target Images

	For the WCCF build, select (*) squashfs.

### Select Base System Components

	Add (*) wireless-tools

### TP-LINK Archer C7 ONLY: Select Kernel Modules

This step is required to include support for the TP-LINK Archer C7 5GHz 802.11a/n/ac radio.  If this step is omitted, when the TP-LINK 5GHz radio is enabled, the 5GHz radio will report (in the Web U/I) *Wireless is disabled or not associated* and will fail to function.  (Note there is no corresponding wireless radio build requirement for the Netgear AP.)

	Wireless Drivers ---> kmod-ath10k (Atheros 802.11ac wireless cards support)


### Select Network Components

	Add Network -> File Transfer -> (*) rsync
	Add Network -> Web Services/Proxies -> (*) uhttpd
	Add Network -> Web Services/Proxies -> (*) uhttpd-mod-lua
	Add Network -> (*) tcpdump
	Add Network -> (*) wccf


### Select LuCI Components

	Add LuCI -> Collections -> (*) luci
	Add LuCI -> Modules -> (*) luci-mod-admin-full
	Add LuCI -> Applications -> (*) luci-app-ddns
	Add LuCI -> Applications -> (*) luci-app-firewall
	Add LuCI -> Applications -> (*) luci-app-ntpc
	Add LuCI -> Applications -> (*) luci-app-qos


### Select Libraries

	Add Libraries -> (*) libexpat
	Add Libraries -> (*) libiw
	Add Libraries -> (*) libuuid


### Select Utilities

	Add Utilities -> Shells -> (*) bash

> See utility scripts here [wccf/qa/utilities/build_*](../qa/utilities) that automate this step (and others).

Save the new config (to file .config).  Then, from the command line, enter:

	make

Note: The final make step above may take a very long time when first executed (even more than an hour, depending on hardware).  Subsequent make commands will take far less time (unless a make clean is run).

The Netgear N600 Model WNDR3800 flash image will appear here:

	bin/ar71xx/openwrt-ar71xx-generic-wndr3800-squashfs-sysupgrade.bin

The TP-LINK Archer C7 flash image will appear here:

	bin/ar71xx/openwrt-ar71xx-generic-archer-c7-v2-squashfs-sysupgrade.bin

NOTE: the TP-LINK Archer C7 requires v2.  An Archer C7 v1 image is also produced by this build sequence, however, the v1 image is unusable.

The new image can be copied onto the AP using scp, or it can be loaded directly using the AP GUI Mgmt Console as a new bootable image. For an example of that process, see [HOWTO_FlashTheNetgearWNDR3800.md](./HOWTO_FlashTheNetgearWNDR3800.md).

NOTE: The above binary filenames are intended for use in flashing an OpenWrt Access Point.  If you have never flashed an OpenWrt image to the device (ie, it is running a factory image from the vendor), then use the following image instead (same directory path) with the vendor-specific 'update firmware' feature of the mgmt console.

When using the vendor-specific management console, the device IP address is often 192.168.0.1 (vs 192.168.1.1 for OpenWrt images) so if you're setting your accessing computer to a static address (an optional choice), be sure to use the 192.168.0.x subnet to access the factor mgmt console. See vendor's documentation for more details, such as for default username/password settings (TP-LINK uses admin/admin, and the flash screen is at System Tools | Firmware Upgrade ).

These files (below) are used the first time only to introduce OpenWrt to an off-the-shelf device:

	openwrt-ar71xx-generic-wndr3800-squashfs-factory.img
	openwrt-ar71xx-generic-archer-c7-v2-squashfs-factory.img

NOTE: the TP-LINK Archer C7 requires v2.  An Archer C7 v1 image is also produced by this build sequence, however, the v1 image is unusable.

NOTE: Original factory-image TP-Link AP devices are set up on the subnet 192.168.0.1 whereas OpenWrt images utilize 192.168.1.1.  Therefore, take care when using browser bookmarks or command-line values to successfully reach the AP.


## Next Steps
This completes the process of building and deploying the WCCF components.  The Access Point is now ready to test and deploy.

For additional details on AP configuration, see the document in this directory entitled [AdminGuide.md](./AdminGuide.md).
