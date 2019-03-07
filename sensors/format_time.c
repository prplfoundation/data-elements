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

#include <stdio.h>
#include <time.h>
#include "format_time.h"

char *format_time(char *outstr, time_t t) {
    struct tm *tstruct = gmtime(&t);
    memset(outstr, 0, TIMESTAMP_LENGTH);
    strftime(outstr, TIMESTAMP_LENGTH, "%Y-%m-%dT%H:%M:%S.000+00:00", tstruct);
    return outstr;
}
