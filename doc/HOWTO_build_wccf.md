# Wireless Common Collection Framework - WCCF

# HOWTO\_build\_wccf

### Version 2.0.0

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

The resulting build product is file `wccf-2.0.0.tar.gz`.  This file along with others are introduced to the OpenWrt build environment to create a flashable image for the Wireless Access Point.

## Next Steps
This completes the process of building the WCCF components.  For additional steps required to build and deploy an OpenWrt image with WCCF, see file:

[wccf/doc/HOWTO_start_here.md](./HOWTO_start_here.md)


