#!/bin/bash 
#
# Copyright (c) 2017 Applied Broadband, Inc., and
#                    Cable Television Laboratories, Inc. ("CableLabs")
#
# A design goal of this script is to be callable eventually from Jenkins.
# Jenkins jobs are usually run from user jenkins - therefore, user
# jenkins would define OPENWRT_BUILD_ROOT in its environment eg - in file
# ~/.bashrc adding:
#
#    export OPENWRT_BUILD_ROOT=$HOME/openwrt_git_clone/openwrt
#
# That said, this script does NOT assume it's run by user jenkins.
#
# Usage: ./build_openwrt.sh [ n [ NOMENU ] ] 
#    where n is the seconds to count-down - n=0 avoids count-down,
#    and including NOMENU avoids the 'make menuconfig' step.

SCRIPT_NAME=$( basename "$0" )
echo -e "\nStarting script $SCRIPT_NAME."

: ${OPENWRT_BUILD_ROOT?"User environment must set var OPENWRT_BUILD_ROOT"}

WCCF_BUILD_PRODUCT=wccf-1.2.0.tar.gz
WCCF_MAKEFILE_TEMPLATE=targets/openwrt/Makefile.template
WCCF_PATCHES_DIR=targets/openwrt/patches
TMP_WCCF=/tmp/__wccf_to_openwrt__

build_now() {
	CUR_DIR=$( pwd )
	echo -e "Changing to $OPENWRT_BUILD_ROOT\n"
	cd $OPENWRT_BUILD_ROOT

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

	make defconfig
	make prereq

	if [ $# -lt 2 ] ; then
		make menuconfig
	fi

	make
	echo -e "\nChanging back to $CUR_DIR"
	cd $CUR_DIR
}

show_count_down () {
	SECS=$1
	echo -ne "\rBuilding OpenWrt in $SECS seconds ... enter Ctrl-C to abort  " # trailing spaces erase longer lines
}

counter=10
if [ $# -gt 0 ]; then
	counter=$1
fi

while (( --counter >= 0 )); do
	show_count_down $counter
	sleep 1
done 

echo ""
build_now

echo "WCCF build products for OpenWrt included from $TMP_WCCF"
echo "Script $SCRIPT_NAME complete."

