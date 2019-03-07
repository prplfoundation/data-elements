/*
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
*/
#ifndef _WCCF_SENSOR_
#define _WCCF_SENSOR_

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include <stdbool.h>
#include "nl80211.h"
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>
#include <endian.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>

#include "ieee80211.h"
#include <net/if.h>

#include <getopt.h>
#include "iwinfo.h"
#include "json.h"
#include "format_time.h"
#include "base64.h"

#endif
