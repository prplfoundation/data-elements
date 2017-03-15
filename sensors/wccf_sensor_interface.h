/*
  #
  # Copyright (c) 2017 Applied Broadband, Inc., and
  #                    Cable Television Laboratories, Inc. ("CableLabs")
  #
*/
#ifndef _WCCF_SENSOR_INTERFACE_
#define _WCCF_SENSOR_INTERFACE_

#include "wccf_sensor.h"

struct interface_args {
    char *if_mac_address;
    json_object *jarray;
};

#endif
