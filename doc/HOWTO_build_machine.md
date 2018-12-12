# Wireless Common Collection Framework - WCCF

# HOWTO\_build\_machine

`Copyright (c) 2017 Applied Broadband, Inc., and`
`                   Cable Television Laboratories, Inc. ("CableLabs")`

The WCCF project integrates important Proactive Network Maintenance capabilities with OpenWrt software elements.  Ubuntu has been found to be highly compatible with OpenWrt sources and therefore has been selected at this time as the reference machine environment for WCCF build and test efforts.

This HOWTO provides a checklist of packages to add to a basic Ubuntu server to create a standard (reference) build machine.  At this time, the specific OS version employed by the WCCF project is Ubuntu 16.04.1 LTS (Long Term Support).

Download ubuntu-16.04.1-server-amd64.iso from here:

[https://www.ubuntu.com/download/server](https://www.ubuntu.com/download/server)

Install this ISO on a physical or virtual machine.  Testing has been done with the following VM profile:

		RAM: 1536 MB
		DISK: 32 GB
		PROCESSORS: 2+ (important: more is better for build performance)

During the Ubuntu iso installation, select "manual package selection" and "standard system utilities."  Then, development tools, a lightweight GUI and other utilities are added after boot, as listed below.

During the installation process, define user wccf (password at your discretion).  Ubuntu server enables this initial user (user wccf in this case) with sudo privileges. Ubuntu does not enable user root until a password is set for root.  Therefore, the initial logon will be as user wccf.

## Setting Password for User root

		$ sudo passwd root
		$ <user-wccf-passwd>
		$ <user-root-passwd>
		$ <user-root-passwd-repeated>

## Networking
Networking must be set up within file /etc/network/interfaces.  Here is a sample only - adjust as appropriate for your infrastructure (esp. enp0s3 and enp0s8 settings):

		$ cat /etc/network/interfaces

		# This file describes the network interfaces available on your system
		# and how to activate them. For more information, see interfaces(5).

		source /etc/network/interfaces.d/*

		# The loopback network interface
		auto lo
		iface lo inet loopback

		# The primary network interface
		auto enp0s3
		iface enp0s3 inet dhcp

		auto enp0s8
		iface enp0s8 inet static
			address 10.10.10.252
			netmask 255.255.255.0
			gateway 10.10.10.1

To modify the interface definitions and effect your changes, take these steps (sudo or root):

		$ vi /etc/network/interfaces
		$ systemctl disable networking.service
		$ systemctl enable networking.service
		$ systemctl start networking.service

## Hostname
Also set the hostname (sudo or root):

		$ hostnamectl set-hostname <my_new_host_name>


Edit /etc/hosts as appropriate (sudo or root).

## Additional Required Software Packages
Additional (required by OpenWrt and/or WCCF) Software Package Installations

Install the following set of software tools and libraries (as root, or with sudo):

		$ apt-get update
		$ apt-get upgrade
		$ apt-get install ssh
		$ systemctl enable ssh.service
		$ systemctl start ssh.service
		$ apt-get install uuid-dev libexpat-dev autoconf libtool
		$ apt-get install git-core build-essential libssl-dev libncurses5-dev unzip
		$ apt-get install subversion mercurial
		$ apt-get install libboost-all-dev
		$ apt-get install --no-install-recommends ubuntu-desktop
                $ apt-get install --no-install-recommends gawk
		$ apt-get install gnome-terminal

Reboot the server to boot up into the GUI desktop (console or VNC session is probably required for GUI).

## Next Steps
This completes the minimum required installation of the WCCF Reference Build Machine OS and User.  For additional steps required to build and deploy WCCF, see file:

[wccf/doc/HOWTO_start_here.md](./HOWTO_start_here.md).

