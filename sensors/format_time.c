/*
  #
  # Copyright (c) 2018 Applied Broadband, Inc., and
  #                    Cable Television Laboratories, Inc. ("CableLabs")
  #
*/

#include <stdio.h>
#include <time.h>
#include "format_time.h"

char *format_time(char *outstr, time_t t) {
    struct tm *tstruct = gmtime(&t);
    memset(outstr, 0, TIMESTAMP_LENGTH);
    strftime(outstr, TIMESTAMP_LENGTH, "%Y-%m-%dT%H:%M:%S.000+00:00", tstruct);
    return outstr;
}
