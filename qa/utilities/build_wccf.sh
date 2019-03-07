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
# File: wccf/qa/utilities/build_wccf.sh
#
# A design goal of this script is to be callable eventually from Jenkins.
# Jenkins jobs are usually run from user jenkins - therefore, user
# jenkins would define WCCF_BUILD_ROOT in its environment eg - in file
# ~/.bashrc adding (for example):
#
#    export WCCF_BUILD_ROOT=$HOME/wccf_git_clone/wccf
#
# That said, this script does NOT assume it's run by user jenkins but
# the above env var must be set prior to script invocation.
#
# Usage: ./build_wccf.sh n # where n=0 avoids count-down

SCRIPT_NAME=$( basename "$0" )
echo -e "\nStarting script $SCRIPT_NAME."

: ${WCCF_BUILD_ROOT?"ERROR: User environment must set var WCCF_BUILD_ROOT. Exiting."}

# This controls the build product handed to OpenWrt build
# and must match the value in wccf/qa/utilities/build_openwrt.sh:
WCCF_VERSION=1.4.0

WCCF_BUILD_PRODUCT=wccf-${WCCF_VERSION}.tar.gz
WCCF_MAKEFILE_TEMPLATE=targets/openwrt/Makefile.template
WCCF_PATCHES_DIR=targets/openwrt/patches

# This is the location where wccf/qa/utilities/build_openwrt.sh looks
# for the wccf build product and related package wccf files to
# include in its build. It must match the value in build_openwrt.sh
TMP_WCCF=/tmp/${USER}/__wccf_to_openwrt__

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

	# Update local WCCF git repo
	git pull

	libtoolize
	aclocal
	autoconf
	automake --add-missing
	automake
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

echo ""
build_now

echo "******************************************************************************"
echo "WCCF build products for OpenWrt inclusion written to $TMP_WCCF"
echo "******************************************************************************"
echo "Script $SCRIPT_NAME complete."

