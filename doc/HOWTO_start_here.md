#Wireless Common Collection Framework - WCCF#

#HOWTO_start_here#

`Copyright (c) 2017 Applied Broadband, Inc., and`
`                   Cable Television Laboratories, Inc. ("CableLabs")`

The Wireless Common Collection Framework (WCCF), Version 1.2.0, assumes a host environment of OpenWrt, branch master.  OpenWrt is described as a Linux distribution for embedded devices (from https://openwrt.org).  

Building and using WCCF assumes strong familiarity with the mechanics of installing OpenWrt and building and deploying OpenWrt images.  No attempt is made to duplicate within the WCCF project the materials required to master those tasks.  However, just for clarity, note that it is not sufficient to download a prepared OpenWrt flash image which has already been created to be flashed onto your Vendor/Model-specific Access Point hardware device.  Rather, you must install the entire OpenWrt build environment and to be able to create new flash images that incorporate the WCCF components.

WCCF Version 1.2.0 has focused development and testing on the Netgear N600 (model WNDR3800) Wireless Access Point device.

##WCCF Build/Deployment/Run OVERVIEW##

More detail is available in other HOWTO files (see below).  Here is a high-level summary of the process.

1. Create a reference build machine with required Ubuntu OS version and development packages and utilities.

2. Download and prepare your OpenWrt software and build environment.

3. Build, deploy and test a Wireless Access Point image PRIOR TO building and deploying WCCF into the Access Point device.  Note: Details on building and flashing OpenWrt AP images is beyond the scope of project WCCF.  For more information on these steps, browse the following resources:

    - https://wiki.openwrt.org/doc/start
    - https://wiki.openwrt.org/doc/howto/start
    - https://wiki.openwrt.org/doc/howto/firstlogin
    - https://openwrt.org/

4. After validating the OpenWrt image build/install/operation on the Access Point device (#3 above), download and build WCCF on the same reference build machine.

5. Move the WCCF build product (tar file), the Makefile template, and the patches subdirectory contents into your previously established OpenWrt build environment and build a new AP image.  Deploy this new AP image (incorporating WCCF) to the AP, reboot the device and begin your WCCF testing.


##WCCF Build/Deployment/Run DETAIL##

**Step 1 - Create the Reference Build Machine**

Due to Ubuntu's high compatibility with OpenWrt, the WCCF project has adopted as the standard (reference) build environment Ubuntu 16.04.1 LTS (Long Term Support version).  While much of the software is portable to some degree, all final build and test is completed on this platform.

The reference build machine iso file is available here (as of Jan 2017):
 
 https://www.ubuntu.com/download/server
 (file ubuntu-16.04.1-server-amd64.iso)

Note that a lightweight GUI is available (and recommended) for installation on that server.  For a WCCF (and OpenWrt) Reference Build Machine setup checklist, see the following HOWTO:

[wccf/doc/HOWTO_build_machine.md](./HOWTO_build_machine.md)

**Steps 2 & 3 - OpenWrt Setup**

See the resouces listed in the Overview section above for help setting up the OpenWrt build environment.  Additionally, review the following HOWTO's for hints on structuring your source trees locally to facilitate moving software components through the entire process.

**Step 4 - Accessing and Building WCCF**
To build and prepare the WCCF components for integration with OpenWrt, see the following HOWTO:

[wccf/doc/HOWTO_build_wccf.md](./HOWTO_build_wccf.md)

**Step 5 - Integrate and Build WCCF Into OpenWrt**

To complete the WCCF--OpenWrt integration and prepare a new flashable AP image, see the following HOWTO:

[wccf/doc/HOWTO_build_flashable_image.md](./HOWTO_build_flashable_image.md)


