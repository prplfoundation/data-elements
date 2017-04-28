#!/bin/bash 
#
# Copyright (c) 2017 Applied Broadband, Inc., and
#                    Cable Television Laboratories, Inc. ("CableLabs")
#
# File: wccf/qa/utilities/build_ap_image.sh
#
# This script builds EITHER the Netgear WNDR3800 image OR the TP-LINK
# Archer C7 image.  It accomplishes this by reusing a .config file
# with all appropriate menu selections made.  These .config files are
# held in the WCCF repository and can be inspected or augmented there.

# A design goal of this script is to be callable eventually from Jenkins.
# Jenkins jobs are usually run from user jenkins - therefore, user
# jenkins would define OPENWRT_BUILD_ROOT in its environment eg - in file
# ~/.bashrc adding (for example):
#
#    export OPENWRT_BUILD_ROOT=$HOME/openwrt_git_clone/openwrt
#
# That said, this script does NOT assume it's run by user jenkins but
# the above env var must be set prior to script invocation.
#

SCRIPT_NAME=$( basename "$0" )
echo -e "\nStarting script $SCRIPT_NAME."

: ${OPENWRT_BUILD_ROOT?"ERROR: User environment must set var OPENWRT_BUILD_ROOT. Exiting."}

if [ $# -lt 1 ]; then
    echo "  usage: ./build_ap_image.sh <target_ap_build_config>"
    echo "example: ./build_ap_image.sh \$WCCF_BUILD_ROOT/qa/utilities/TP-LINK.config"
    exit 1
fi

AP_CONFIG=$1
if [ ! -f $AP_CONFIG ]; then
    echo "Designated build config does not exist: $AP_CONFIG"
    exit 2
fi

# Before we start changing directory, move the build config into place
cp $AP_CONFIG $OPENWRT_BUILD_ROOT/.config

# This identifies the build product created by WCCF build
# and must match the value in wccf/qa/utilities/build_wccf.sh:
WCCF_VERSION=1.4.0

WCCF_BUILD_PRODUCT=wccf-${WCCF_VERSION}.tar.gz
WCCF_MAKEFILE_TEMPLATE=targets/openwrt/Makefile.template
WCCF_PATCHES_DIR=targets/openwrt/patches

# This is the location where wccf/qa/utilities/build_wccf.sh
# puts its build product and related package wccf files for
# the OpenWrt image build to copy into its build.  It must
# match the value used in build_openwrt.sh.
TMP_WCCF=/tmp/${USER}/__wccf_to_openwrt__

build_now() {
	CUR_DIR=$( pwd )
	echo -e "Changing to $OPENWRT_BUILD_ROOT\n"
	cd $OPENWRT_BUILD_ROOT

	# Update local OpenWrt git repo
	git pull

	mkdir -p dl/
	cp $TMP_WCCF/$WCCF_BUILD_PRODUCT dl/
	mkdir -p package/network/wccf
	cp $TMP_WCCF/Makefile package/network/wccf/
        cp -r $TMP_WCCF/patches package/network/wccf/

	./scripts/feeds update -a
	./scripts/feeds install -a

	./scripts/feeds update packages luci
	./scripts/feeds install -a -p luci
	./scripts/feeds update packages libexpat
	./scripts/feeds install -a -p libexpat

	# using predefined config: make defconfig
	# using predefined config: make prereq
	# using predefined config: make menuconfig

	echo "Running OpenWrt make"
	make
	echo "Choose from recent OpenWrt image files:"
	find `pwd` -mmin -30 -type f -name "openwrt*sysupgrade*.bin" | xargs ls -lart
	echo -e "\nChanging back to $CUR_DIR"
	cd $CUR_DIR
}

echo ""
build_now

echo "******************************************************************************"
echo "WCCF build products for OpenWrt included from $TMP_WCCF"
echo "******************************************************************************"
echo "Script $SCRIPT_NAME complete."

