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
#include "wccf_sensor_event.h"
#define MAX_SCAN_BUF_LEN 200

int get_interface_mac(char *if_name, char *mac_string)
{
    int fd;
    struct ifreq ifr;
    unsigned char *mac = NULL;

    memset(&ifr, 0, sizeof(ifr));

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , if_name , IFNAMSIZ-1);

    if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr)) {
        mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
        sprintf(mac_string, "%02x:%02x:%02x:%02x:%02x:%02x" ,
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

    close(fd);

    /*
     * TODO - handle error responses
     */
    return 0;
}

void remove_all_chars(char* str, char c)
{
    char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}

int main(int argc, char *argv[])
{
    static struct option long_options[] = {
        {"interface-name", required_argument, 0, 'i'},
        {"output-dir",     required_argument, 0, 'o'},
        {0,0,0,0}
    };
    int option_index = 0, opt = 0;
    int ret = 0;
    char iface_name[80], output_dir[512];
    char file_name[512];
    char tstamp[16], dev_mac[20];
    FILE *fp;
    char iface_mac[18];
    char scan_mac[18];



    while (1) {
        opt = getopt_long(argc, argv, "i:o:", long_options, &option_index);

        if (opt == -1)
            break;
        switch (opt)
            {
            case 'i':
                strcpy(iface_name, optarg);
                break;
            case 'o':
                strcpy(output_dir, optarg);
                break;
            default:
                return 1;
            }
    }

    int matchcount = 18;
    regmatch_t result[19];
    memset(result, 0, sizeof(regmatch_t)*19);

    regex_t event_output1;
    char *event_pattern1 =
        "^([0-2][0-9]:[0-5][0-9]:[0-5][0-9].[0-9]+) ([0-9]+us) tsft ([0-9].[0-9] Mb/s) ([0-9]+) MHz (11a|11b) (-?[0-9]+)dB signal antenna [0-9]+ [0-9]+us Assoc Request \\([a-zA-Z0-9_\\.-]*\\)* \\[(.*)+ Mbit\\].*$";
    //19:27:37.327410 770499105us tsft 6.0 Mb/s 5180 MHz 11a -57dB signal antenna 0 60us Assoc Request (WccfOpenWrt-5) [6.0* 9.0 12.0* 18.0 24.0* 36.0 48.0 54.0 Mbit]

    ret = regcomp(&event_output1, event_pattern1, REG_EXTENDED);
    if (ret) {
        printf("Error compiling expression 1: %d\n", ret);
        exit(1);
    }

    regex_t event_output2;
    char *event_pattern2 =
        "^([0-2][0-9]:[0-5][0-9]:[0-5][0-9].[0-9]+) ([0-9]+us) tsft ([0-9].[0-9] Mb/s) ([0-9]+) MHz (11a|11b) (-?[0-9]+)dB signal \\[bit 29\\] [0-9]+us Assoc Request \\([a-zA-Z0-9_\\.-]*\\)* \\[(.*)+ Mbit\\].*$";
    //19:27:37.327410 770499105us tsft 6.0 Mb/s 5180 MHz 11a -57dB signal [bit 29] 60us Assoc Request (WccfOpenWrt-5) [6.0* 9.0 12.0* 18.0 24.0* 36.0 48.0 54.0 Mbit]

    ret = regcomp(&event_output2, event_pattern2, REG_EXTENDED);
    if (ret) {
        printf("Error compiling expression 2: %d\n", ret);
        exit(1);
    }

    char scan_buf[MAX_SCAN_BUF_LEN];
    int scan_idx = 0;
    regex_t staddr_output;
    char *staddr_pattern = "0x00[1-6]0:  ([0-9a-f]{2})([0-9a-f]{2}) ([0-9a-f]{2})([0-9a-f]{2}) ([0-9a-f]{2})([0-9a-f]{2}) ([0-9a-f]{2})([0-9a-f]{2}) ([0-9a-f]{2})([0-9a-f]{2}) ([0-9a-f]{2})([0-9a-f]{2}) ([0-9a-f]{2})([0-9a-f]{2}) ([0-9a-f]{2})([0-9a-f]{2})  .*$";
    //	0x0030:  9093 1100 1400 000d 5763 6366 4f70 656e  ........WccfOpen

    ret = regcomp(&staddr_output, staddr_pattern, REG_EXTENDED);
    if (ret) {
        printf("Error compiling address expression: %d\n", ret);
        exit(1);
    }

    FILE *in;
    extern FILE *popen();
    char *buff = NULL;
    size_t len = 0;
    ssize_t read = 0;
    char *line = (char *)malloc(256);
    size_t lineLen = 0;
    char cmd_str[512];
    char *m_dev;

    char *cmd_fmt = "tcpdump -i %s -nnvXXSs 0 -n ether dst %s and type mgt subtype assoc-req";

    //
    // shortcut to reference monitor device created by init script...
    //
    if (0 == strcmp(iface_name, "wlan0"))
        m_dev = "mon0";
    else if  (0 == strcmp(iface_name, "wlan1"))
        m_dev = "mon1";

    get_interface_mac(iface_name, iface_mac);
    memcpy(scan_mac, iface_mac, 18);
    remove_all_chars(scan_mac, ':');
    printf("scan_mac = %s\n", scan_mac);

    sprintf(cmd_str, cmd_fmt, m_dev, iface_mac);

    if (NULL == (in = popen(cmd_str, "r"))) {
        printf("error executing tcpdump command: %s\n", strerror(errno));
        exit(1);
    }

    int new_event = 0;

    //
    // turn off stdio buffering
    //
    setvbuf(in, NULL, _IONBF, 0);

    while (getline(&line, &lineLen, in) != -1) {
        json_object *jsta_array;
        json_object *jsta;

        memset(result, 0, sizeof(result));

        //
        // try to match the 5GHz event line
        //
        ret = regexec(&event_output1, line, matchcount+1, result, 0);
        if (!ret) {
            new_event = 1;
            memset(scan_buf, 0, MAX_SCAN_BUF_LEN);
            scan_idx = 0;

            jsta_array = json_object_new_array();
            jsta = json_object_new_object();

            for (int i=1; i < 11; i++) {
                if (result[i].rm_so == result[i].rm_eo)
                    continue;
                int rlen = result[i].rm_eo - result[i].rm_so;
                char *res = calloc(1, rlen+1);
                strncpy(res, &line[result[i].rm_so], rlen);
                res[rlen] = 0;
                if (i == 1) {
                    json_object *jtime = json_object_new_string(res);
                    json_object_object_add(jsta, "EventTime", jtime);
                    json_object *jevent = json_object_new_string("AssocReq");
                    json_object_object_add(jsta, "EventType", jevent);
                    free(res);
                }
                else if (i == 4) {
                    int cfreq = atoi(res);
                    int channel = 0;
                    json_object *jcfreq = json_object_new_int(cfreq);
                    json_object_object_add(jsta, "ConnectFrequency", jcfreq);

                    if (cfreq == 2484)
                        channel = 14;
                    else if (cfreq < 2484)
                        channel = (cfreq - 2407) / 5;
                    else if (cfreq >= 4910 && cfreq <= 4980)
                        channel = (cfreq - 4000) / 5;
                    else if (cfreq <= 45000)
                        channel = (cfreq - 5000) / 5;
                    else if (cfreq >= 58320 && cfreq <= 64800)
                        channel = (cfreq - 56160) / 2160;

                    json_object *jchan = json_object_new_int(channel);
                    json_object_object_add(jsta, "Channel", jchan);
                    free(res);
                }
                else if (i == 5) {
                    json_object *jmode = json_object_new_string(res);
                    json_object_object_add(jsta, "ConnectMode", jmode);
                    free(res);
                }
                else if (i == 6) {
                    int csig = atoi(res);
                    json_object *jcsig = json_object_new_int(csig);
                    json_object_object_add(jsta, "ConnectSignal", jcsig);
                    free(res);
                }
                else if (i == 7) {
                    json_object *jbrate = json_object_new_array();
                    char *brpt = res;
                    brpt = strtok(res, " ");
                    while (brpt != NULL) {
                        float rate = 0.0;
                        if (brpt[strlen(brpt)] == '*')
                            brpt[strlen(brpt)] = 0;
                        rate = atof(brpt);
                        json_object *jbr = json_object_new_double(rate);
                        json_object_array_add(jbrate, jbr);
                        brpt = strtok(NULL, " ");
                    }
                    free(res);
                    json_object_object_add(jsta, "SupportedBitrate", jbrate);
                }
            }
            continue;
        }

        memset(result, 0, sizeof(result));

        //
        // try to match the 2.4GHz event line
        //
        ret = regexec(&event_output2, line, matchcount+1, result, 0);
        if (!ret) {
            new_event = 1;
            memset(scan_buf, 0, MAX_SCAN_BUF_LEN);
            scan_idx = 0;

            jsta_array = json_object_new_array();
            jsta = json_object_new_object();

            for (int i=1; i < 11; i++) {
                if (result[i].rm_so == result[i].rm_eo)
                    continue;
                int rlen = result[i].rm_eo - result[i].rm_so;
                char *res = calloc(1, rlen+1);
                strncpy(res, &line[result[i].rm_so], rlen);
                res[rlen] = 0;
                if (i == 1) {
                    json_object *jtime = json_object_new_string(res);
                    json_object_object_add(jsta, "LastConnectTime", jtime);
                    json_object *jevent = json_object_new_string("AssocReq");
                    json_object_object_add(jsta, "EventType", jevent);
                    free(res);
                }
                else if (i == 4) {
                    int cfreq = atoi(res);
                    int channel = 0;
                    json_object *jcfreq = json_object_new_int(cfreq);
                    json_object_object_add(jsta, "ConnectFrequency", jcfreq);

                    if (cfreq == 2484)
                        channel = 14;
                    else if (cfreq < 2484)
                        channel = (cfreq - 2407) / 5;
                    else if (cfreq >= 4910 && cfreq <= 4980)
                        channel = (cfreq - 4000) / 5;
                    else if (cfreq <= 45000)
                        channel = (cfreq - 5000) / 5;
                    else if (cfreq >= 58320 && cfreq <= 64800)
                        channel = (cfreq - 56160) / 2160;

                    json_object *jchan = json_object_new_int(channel);
                    json_object_object_add(jsta, "Channel", jchan);
                    free(res);
                }
                else if (i == 5) {
                    json_object *jmode = json_object_new_string(res);
                    json_object_object_add(jsta, "ConnectMode", jmode);
                    free(res);
                }
                else if (i == 6) {
                    int csig = atoi(res);
                    json_object *jcsig = json_object_new_int(csig);
                    json_object_object_add(jsta, "ConnectSignal", jcsig);
                    free(res);
                }
                else if (i == 7) {
                    json_object *jbrate = json_object_new_array();
                    char *brpt = res;
                    brpt = strtok(res, " ");
                    while (brpt != NULL) {
                        float rate = 0.0;
                        if (brpt[strlen(brpt)] == '*')
                            brpt[strlen(brpt)] = 0;
                        rate = atof(brpt);
                        json_object *jbr = json_object_new_double(rate);
                        json_object_array_add(jbrate, jbr);
                        brpt = strtok(NULL, " ");
                    }
                    free(res);
                    json_object_object_add(jsta, "SupportedBitrate", jbrate);
                }
            }
            continue;
        }

        memset(result, 0, sizeof(result));

        ret = regexec(&staddr_output, line, matchcount+1, result, 0);
        if (!ret) {
            char staddr[18];

            if (new_event == 0) {
                continue;
            }

            for (int i=1; i < 19; i++) {
                if (result[i].rm_so == result[i].rm_eo)
                    continue;
                int rlen = result[i].rm_eo - result[i].rm_so;
                strncpy((char *)&scan_buf[scan_idx], &line[result[i].rm_so], rlen);
                scan_idx += rlen;
                if  (result[i+1].rm_so == result[i+1].rm_eo)
                    break;
            }

            if (line[5] != '6')
                continue;

            char *dest_match = strstr(scan_buf, scan_mac);
            if (dest_match) {
                int offset = 12;

                memset(staddr, 0, 18);

                staddr[0]  = dest_match[offset++];
                staddr[1]  = dest_match[offset++];
                staddr[2]  = ':';
                staddr[3]  = dest_match[offset++];
                staddr[4]  = dest_match[offset++];
                staddr[5]  = ':';
                staddr[6]  = dest_match[offset++];
                staddr[7]  = dest_match[offset++];
                staddr[8]  = ':';
                staddr[9]  = dest_match[offset++];
                staddr[10] = dest_match[offset++];
                staddr[11] = ':';
                staddr[12] = dest_match[offset++];
                staddr[13] = dest_match[offset++];
                staddr[14] = ':';
                staddr[15] = dest_match[offset++];
                staddr[16] = dest_match[offset++];
            }

            json_object *jstamac = json_object_new_string(staddr);
            json_object_object_add(jsta, "MACAddress", jstamac);

            json_object_array_add(jsta_array, jsta);

            json_object *jevent = json_object_new_object();

            memset(iface_mac, 0, 18);
            ret = get_interface_mac(iface_name, iface_mac);

            json_object *japmac = json_object_new_string(iface_mac);
            json_object_object_add(jevent, "ID", japmac);

            time_t current_time = time(0);
            char timestr[TIMESTAMP_LENGTH];
            (void)format_time(timestr, current_time);
            json_object *jctime = json_object_new_string(timestr);
            json_object_object_add(jevent, "eventTime", jctime);

            json_object_object_add(jevent, "Event", jsta_array);
            /*
             * for now output the main json structure anyway
             */
            json_object *jwifi = json_object_new_object();
            json_object_object_add(jwifi, "Wifi", jevent);
            json_object *jdevice = json_object_new_object();
            json_object_object_add(jdevice, "wfa-dataelements:AssociationEvent", jwifi);

            struct tm *tstruct = gmtime(&current_time);
            strftime(tstamp, 16, "%Y%m%d%H%M%S", tstruct);

            get_interface_mac("br-wan", dev_mac);

            sprintf(file_name, "%s/%s_event-%s_%s00.json", output_dir, dev_mac, iface_name, tstamp);
            fp = fopen(file_name, "wb");
            if (!fp) {
                fprintf(stderr, "Cant open output file: %s", file_name);
                return 1;
            }

            fprintf(fp, "%s\n", json_object_to_json_string(jdevice));
            fclose(fp);

            new_event = 0;
        }
    }

    return 0;
}
