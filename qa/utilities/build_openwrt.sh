#!/bin/bash 
#
# Copyright (c) 2019 Cable Television Laboratories, Inc.
# Licensed under the BSD+Patent (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#          https://opensource.org/licenses/BSDplusPatent
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# File: wccf/qa/utilities/build_openwrt.sh
#
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
# Usage: ./build_openwrt.sh [ n [ NOMENU ] ] 
#    where n is the seconds to count-down - n=0 avoids count-down,
#    and including NOMENU avoids the 'make menuconfig' step.

SCRIPT_NAME=$( basename "$0" )
echo -e "\nStarting script $SCRIPT_NAME."

: ${OPENWRT_BUILD_ROOT?"ERROR: User environment must set var OPENWRT_BUILD_ROOT. Exiting."}

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

	make defconfig
	make prereq

	if [ $# -lt 2 ] ; then
		make menuconfig
	fi

	echo "Running OpenWrt make"
	make
	echo "Choose from recent OpenWrt image files:"
	find `pwd` -mmin -30 -type f -name "openwrt*sysupgrade*.bin" | xargs ls -lart
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

echo "******************************************************************************"
echo "WCCF build products for OpenWrt included from $TMP_WCCF"
echo "******************************************************************************"
echo "Script $SCRIPT_NAME complete."

