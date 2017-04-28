/*
  #
  # Copyright (c) 2017 Applied Broadband, Inc., and
  #                    Cable Television Laboratories, Inc. ("CableLabs")
  #
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
};
  
#endif
