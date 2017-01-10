#!/bin/bash 
#
# Copyright (c) 2017 Applied Broadband, Inc., and
#                    Cable Television Laboratories, Inc. ("CableLabs")
#
# A design goal of this script is to be callable eventually from Jenkins.
# Jenkins jobs are usually run from user jenkins - therefore, user
# jenkins would define WCCF_BUILD_ROOT in its environment eg - in file
# ~/.bashrc adding:
#
#    export WCCF_BUILD_ROOT=$HOME/wccf_git_clone/wccf
#
# That said, this script does NOT assume it's run by user jenkins.
#
# Usage: ./build_wccf.sh n # where n=0 avoids count-down

SCRIPT_NAME=$( basename "$0" )
echo -e "\nStarting script $SCRIPT_NAME."

: ${WCCF_BUILD_ROOT?"User environment must set var WCCF_BUILD_ROOT"}

WCCF_BUILD_PRODUCT=wccf-1.2.0.tar.gz
WCCF_MAKEFILE_TEMPLATE=targets/openwrt/Makefile.template
WCCF_PATCHES_DIR=targets/openwrt/patches
TMP_WCCF=/tmp/__wccf_to_openwrt__

# The git pull cmd in function build_now works without credentials
# because the local (persistent) repository is created with this:
#
#    git clone https://<username>:<password>@github.com/appliedbroadband/wccf.git
#
# where <username>:<password> is an existing credentialed user (need not be
# user jenkins).
build_now() {
	CUR_DIR=$( pwd )
	echo -e "Changing to $WCCF_BUILD_ROOT\n"
	cd $WCCF_BUILD_ROOT

	git pull

	libtoolize
	aclocal
	automake --add-missing
	automake
	autoconf
	./configure
	make dist

	mkdir -p $TMP_WCCF
	rm -rf $TMP_WCCF/*

	cp $WCCF_BUILD_PRODUCT $TMP_WCCF/$WCCF_BUILD_PRODUCT
	cp $WCCF_MAKEFILE_TEMPLATE $TMP_WCCF/Makefile
	cp -r $WCCF_PATCHES_DIR $TMP_WCCF/

	echo -e "\nChanging back to $CUR_DIR"
	cd $CUR_DIR
}

show_count_down () {
	SECS=$1
	echo -ne "\rBuilding WCCF in $SECS seconds ... enter Ctrl-C to abort  " # trailing spaces erase longer lines
}

counter=10
if [ $# -gt 0 ]; then
	counter=$1
fi

while (( --counter >= 0 )); do
	show_count_down $counter
	sleep 1
done 

build_now

echo "WCCF build products for OpenWrt inclusion written to $TMP_WCCF"
echo "Script $SCRIPT_NAME complete."

