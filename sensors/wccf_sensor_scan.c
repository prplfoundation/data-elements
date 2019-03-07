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
#include "wccf_sensor_scan.h"

int get_interface_mac(const char *if_name,
                      char *mac_string)
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

int dump_scan(const struct iwinfo_ops *iw,
              const char *ifname,
              json_object *jobj)
{
    int i, x, len, ret;
    char buf[IWINFO_BUFSIZE];
    char ap_mac_str[20];
    struct iwinfo_scanlist_entry *e;

    if (iw->scanlist(ifname, buf, &len)) {
        printf("Scanning not possible\n\n");
        return 1;
    }
    else if (len <= 0) {
        printf("No scan results\n\n");
        return 1;
    }

    int if_index = if_nametoindex(ifname);

    /*
     * single current c-epoch of data sample
     */
    time_t current_time = time(0);

    ret = get_interface_mac(ifname, ap_mac_str);

    for (i = 0, x = 1; i < len; i += sizeof(struct iwinfo_scanlist_entry), x++) {
        json_object *jdev = json_object_new_object();
        char mac_str[20];
        e = (struct iwinfo_scanlist_entry *) &buf[i];

        json_object *jifname = json_object_new_string(ifname);
        json_object_object_add(jdev, "APName", jifname);

        json_object *jifmac = json_object_new_string(ap_mac_str);
        json_object_object_add(jdev, "ID", jifmac);

        //json_object *jifidx = json_object_new_int(if_index);
        //json_object_object_add(jdev, "APIfIndex", jifidx);

        /*
         * output interface index as b64
         */
        const char *binidx = (char *)&(if_index);
        char b64idx[(sizeof(int)*2)+1];
        int len = Base64encode(b64idx, binidx, sizeof(int));
        json_object *jifindex = json_object_new_string(b64idx);
        json_object_object_add(jdev, "APIfIndex", jifindex);

        char timestr[TIMESTAMP_LENGTH];
        (void)format_time(timestr, current_time);
        json_object *jctime = json_object_new_string(timestr);
        json_object_object_add(jdev, "TimeStamp", jctime);

        memset(mac_str, 0, sizeof(mac_str));
        sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x",
                e->mac[0],e->mac[1],e->mac[2],e->mac[3],e->mac[4],e->mac[5]);
        json_object *jremmac = json_object_new_string(mac_str);
        json_object_object_add(jdev,"BSSID", jremmac);

        json_object *jssid = json_object_new_string(e->ssid);
        json_object_object_add(jdev, "SSID", jssid);

        json_object *jsig = json_object_new_double((int)e->signal - 256.0);
        json_object_object_add(jdev, "SignalStrength", jsig);

        json_object *jchan = json_object_new_int((int)e->channel);
        json_object_object_add(jdev, "Channel", jchan);

        int frequency = 0;
        if (e->channel < 14) {
            frequency = (e->channel * 5) + 2407;
        }
        else if (e->channel == 14) {
            frequency = 2484;
        }
        else if (e->channel >= 182 && e->channel <= 196) {
            frequency = (e->channel * 5) + 4000;
        }
        else {
            frequency = (e->channel * 5) + 5000;
        }
        json_object *jfreq = json_object_new_int(frequency);
        json_object_object_add(jdev, "Frequency", jfreq);

        json_object_array_add(jobj, jdev);
    }
    /*
     * TODO - handle error responses
     */
    return 0;
}

int main(int argc, char *argv[])
{
    static struct option long_options[] = {
        {"interface-name", required_argument, 0, 'i'},
        {"output-dir",     required_argument, 0, 'o'},
        {0,0,0,0}
    };
    int option_index = 0, opt = 0;
    const struct iwinfo_ops *iw;
    json_object *jarray = NULL;
    FILE *fp;
    char iface_name[20], output_dir[256], file_name[512];
    char tstamp[16], dev_mac[20];

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

    iw = iwinfo_backend(iface_name);
    if (!iw) {
        fprintf(stderr, "No such wireless device found: %s\n", iface_name);
        return 1;
    }
    jarray = json_object_new_array();

    dump_scan(iw, iface_name, jarray);

    /*
     * for now output the main json structure anyway
     */
    json_object *jstation = json_object_new_object();
    json_object_object_add(jstation, "Scan", jarray);
    json_object *jwifi = json_object_new_object();
    json_object_object_add(jwifi, "Wifi", jstation);
    json_object *jdevice = json_object_new_object();
    json_object_object_add(jdevice, "Device", jwifi);

    time_t t = 0;
    time(&t);
    struct tm *tstruct = gmtime(&t);
    strftime(tstamp, 16, "%Y%m%d%H%M%S", tstruct);
    get_interface_mac("br-wan", dev_mac);

    sprintf(file_name, "%s/%s_scan-%s_%s00.json", output_dir, dev_mac,
            iface_name, tstamp);
    fp = fopen(file_name, "wb");
    if (!fp) {
        fprintf(stderr, "Cant open output file: %s", file_name);
        return 1;
    }

    fprintf(fp, "%s\n", json_object_to_json_string(jdevice));
    fclose(fp);

    return 0;
}

