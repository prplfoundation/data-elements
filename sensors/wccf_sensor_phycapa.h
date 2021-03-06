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
#ifndef _WCCF_SENSOR_PHYCAPA_
#define _WCCF_SENSOR_PHYCAPA_

#include "wccf_sensor.h"
#define BIT(x) (1ULL<<(x))

struct phycapa_args {
    char *if_mac_address;
    json_object *jarray;
    int last_band;
    bool width_40;
    bool width_80;
    bool width_160;
    int if_index;
};
  
#endif
