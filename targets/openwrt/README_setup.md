#Steps to build wccf 1.2.0 in OpenWrt Branch master 2017/1/10#

`Copyright (c) 2017 Applied Broadband, Inc., and`
`                   Cable Television Laboratories, Inc. ("CableLabs")`

Ubuntu 4.4.0-59-generic x86_64 (Ubuntu server 16.04.1 LTS)
 
    $ sudo apt-get update
    $ sudo apt-get install uuid-dev libexpat-dev autoconf
    $ sudo apt-get install git-core build-essential libssl-dev libncurses5-dev unzip
    $ sudo apt-get install subversion mercurial

    $ git clone git://github.com/openwrt/openwrt.git

    $ cd openwrt
    $ ./scripts/feeds update -a
    $ ./scripts/feeds install -a
    
From [http://wiki.openwrt.org/doc/howto/build](http://wiki.openwrt.org/doc/howto/build):
    
In the OpenWrt build account:

SCP the build package to ```~/``` (it should be named ```wccf-1.2.0.tar.gz```)

Copy the file ```targets/openwrt/Makefile.template``` to ```~/```

Copy the ```targets/openwrt/patches``` directory contents to ```~/patches```

Enter these commands:

    $ cd <openwrt-loc>/openwrt
    $ mkdir dl (if it isn’t there already)
    $ cp ~/wccf-1.2.0.tar.gz dl/
    $ mkdir -p package/network/wccf
    $ cp ~/Makefile.template package/network/wccf/Makefile # Note renamed
    $ cp -r ~/patches package/network/wccf/
    
    $ ./scripts/feeds update packages luci
    $ ./scripts/feeds install -a -p luci
    $ ./scripts/feeds update packages libexpat
    $ ./scripts/feeds install -a -p libexpat
    
    $ make defconfig
    $ make prereq
    $ make menuconfig
    
Using the Menu-based configuration GUI, make appropriate selections for your target OpenWrt router/hardware from these menu items -

- Target System
- Subtarget
- Target Profile

For the Web U/I Mgmt Console, select:

- Network->Web Servers/Proxies->uhttpd
- Network->Web Servers/Proxies->uhttpd-mod-lua
- LuCI->Collections->luci
- LuCI->Modules->luci-mod-admin-full

Other desired options, for example:

- LuCI->Applications->luci-app-ddns
- LuCI->Applications->luci-app-firewall
- LuCI->Applications->luci-app-ntpc
- LuCI->Applications->luci-app-qos


Specifically, make these selections to add support for WCCF -

- Libraries->libiw
- Network->wccf
- Utilities->bash

Save the new configuration to .config. Then, from the command line, enter:

    $ make

The OpenWrt image will be written in the standard location.  For example, for Netgear N600 Model WNDR3800, here:

    .../openwrt/bin/ar71xx/openwrt-ar71xx-generic-wndr3800-squashfs-sysupgrade.bin

