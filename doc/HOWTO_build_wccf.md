# Wireless Common Collection Framework - WCCF

# HOWTO\_build\_wccf

### Version 1.4.0

`Copyright (c) 2017 Applied Broadband, Inc., and`
`                   Cable Television Laboratories, Inc. ("CableLabs")`

This HOWTO assumes you have previously set up a reference build machine (see [wccf/doc/HOWTO_build_machine.md](./HOWTO_build_machine.md)), and have installed, built and tested OpenWrt locally including creating and flashing an AP image (see [wccf/doc/HOWTO_start_here.md](./HOWTO_start_here.md)).

Obtain a copy of the WCCF source from the appropriate git repository (example only shown here):

		$ pwd
		/home/wccf
		$ mkdir ~/wccf-source
		$ cd ~/wccf-source
		$ git clone https://github.com/appliedbroadband/wccf.git

> See utility scripts here [wccf/qa/utilities/build_*](../qa/utilities) that automate these steps (and others).

With a fresh git clone of wccf, enter the following steps on a fully prepared build machine (see [wccf/doc/HOWTO_build_machine.md](./HOWTO_build_machine.md)):

		$ cd ~/wccf-source/wccf
		$ libtoolize
		$ aclocal
		$ autoconf
		$ automake --add-missing
		$ automake
		$ ./configure
		$ make dist

The resulting build product is file `wccf-1.4.0.tar.gz`.  This file along with others are introduced to the OpenWrt build environment to create a flashable image for the Wireless Access Point.

## Next Steps
This completes the process of building the WCCF components.  For additional steps required to build and deploy an OpenWrt image with WCCF, see file:

[wccf/doc/HOWTO_start_here.md](./HOWTO_start_here.md)


