/*
  #
  # Copyright (c) 2017 Applied Broadband, Inc., and
  #                    Cable Television Laboratories, Inc. ("CableLabs")
  #
*/
#include "wccf_sensor_interface.h"

/*
 * Callback routine to handle NL80211 responses to a NL80211_CMD_GET_INTERFACE
 * request.
 */
static int get_interface_cb(struct nl_msg *msg, void* arg)
{
    char mac_str[20], device[10];
    unsigned char mdat[6];
    json_object *jarray = ((struct interface_args *)arg)->jarray;
    char *if_mac_address = ((struct interface_args *)arg)->if_mac_address;
    struct nlmsghdr* hdr = nlmsg_hdr(msg);
    struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];
    struct genlmsghdr *gnlhdr = (struct genlmsghdr *)nlmsg_data(hdr);

    nla_parse(tb_msg, NL80211_ATTR_MAX, genlmsg_attrdata(gnlhdr, 0),
              genlmsg_attrlen(gnlhdr, 0), NULL);

    json_object *jdev = json_object_new_object();

    /*
     * look up device name using interface index
     */
    if (tb_msg[NL80211_ATTR_IFINDEX]) {
        if_indextoname(nla_get_u32(tb_msg[NL80211_ATTR_IFINDEX]), device);

        json_object *jdevname = json_object_new_string(device);
        json_object_object_add(jdev, "APName", jdevname);
    }
    
    /*
     * repeat AP Interface MAC address
     */
    if (tb_msg[NL80211_ATTR_MAC]) {
        char mac_str[20];
        unsigned char mdat[6];
        memset(mac_str, 0, sizeof(mac_str));
        memcpy(mdat, nla_data(tb_msg[NL80211_ATTR_MAC]), sizeof(mdat));
        sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x",
                mdat[0],mdat[1],mdat[2],mdat[3],mdat[4],mdat[5]);
        json_object *japmac = json_object_new_string(mac_str);
        json_object_object_add(jdev, "APMACAddress", japmac);
    }

    /*
     * output interface index as numeric
     */
    if (tb_msg[NL80211_ATTR_IFINDEX]) {
        json_object *jifindex =
            json_object_new_int(nla_get_u32(tb_msg[NL80211_ATTR_IFINDEX]));
        json_object_object_add(jdev, "APIfIndex", jifindex);
    }
  
    //if (tb_msg[NL80211_ATTR_SSID]) {
    //    print_ssid_escaped(nla_len(tb_msg[NL80211_ATTR_SSID]),
    //                       nla_data(tb_msg[NL80211_ATTR_SSID]));
    //    printf("NOT IMPLEMENTED\n");
    //}
    
    /*
     * current c-epoch of data sample
     */
    time_t current_time = time(0);

    json_object *jctime = json_object_new_int(current_time);
    json_object_object_add(jdev, "CurrentUTCTime", jctime);
  
    if (tb_msg[NL80211_ATTR_WIPHY]) {

        if (tb_msg[NL80211_ATTR_WIPHY_CHANNEL_TYPE]) {
            enum nl80211_channel_type channel_type;
            json_object *jctype = NULL;
        
            channel_type = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY_CHANNEL_TYPE]);
            switch (channel_type) {
            case NL80211_CHAN_NO_HT:
                jctype = json_object_new_string("NO HT");
                break;
            case NL80211_CHAN_HT20:
                jctype = json_object_new_string("HT20");
                break;
            case NL80211_CHAN_HT40MINUS:
                jctype = json_object_new_string("HT40-");
                break;
            case NL80211_CHAN_HT40PLUS:
                jctype = json_object_new_string("HT40+");
                break;
            default:
                jctype = json_object_new_string("unknown");
            }
            json_object_object_add(jdev, "HtChannelType", jctype);
        }
    
        if (tb_msg[NL80211_ATTR_CHANNEL_WIDTH]) {
            enum nl80211_chan_width width =
                nla_get_u32(tb_msg[NL80211_ATTR_CHANNEL_WIDTH]);
            json_object *jcwidth = NULL, *jcenter1 = NULL, *jcenter2 = NULL;
        
            switch (width) {
            case NL80211_CHAN_WIDTH_20_NOHT:
                jcwidth = json_object_new_string("20NoHT");
                break;
            case NL80211_CHAN_WIDTH_20:
                jcwidth = json_object_new_string("20");
                break;
            case NL80211_CHAN_WIDTH_40:
                jcwidth = json_object_new_string("40");
                break;
            case NL80211_CHAN_WIDTH_80:
                jcwidth = json_object_new_string("80");
                break;
            case NL80211_CHAN_WIDTH_80P80:
                jcwidth = json_object_new_string("8080");
                break;
            case NL80211_CHAN_WIDTH_160:
                jcwidth = json_object_new_string("160");
                break;
            case NL80211_CHAN_WIDTH_5:
                jcwidth = json_object_new_string("5");
                break;
            case NL80211_CHAN_WIDTH_10:
                jcwidth = json_object_new_string("10");
                break;
            default:
                jcwidth = json_object_new_string("unknown");
            }

            json_object_object_add(jdev, "ChannelWidth", jcwidth);
        
            if (tb_msg[NL80211_ATTR_CENTER_FREQ1]) {
                jcenter1 = json_object_new_int(nla_get_u32(tb_msg[NL80211_ATTR_CENTER_FREQ1]));
                json_object_object_add(jdev, "CenterFreq1", jcenter1);
            }
        
            if (tb_msg[NL80211_ATTR_CENTER_FREQ2]) {
                jcenter2 = json_object_new_int(nla_get_u32(tb_msg[NL80211_ATTR_CENTER_FREQ2]));
                json_object_object_add(jdev, "CenterFreq2", jcenter2);
            }
        }
    }
    
    json_object_array_add(jarray, jdev);
  
    return 0;
}

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

int main(int argc, char *argv[])
{
    static struct option long_options[] = {
        {"output-dir",     required_argument, 0, 'o'},
        {0,0,0,0}
    };
    int option_index = 0, opt = 0;
    int ret = 0;
    int flags = 773; //OK ?
    struct interface_args ifc_args;
    struct nl_sock *nls = nl_socket_alloc();
    char interface_mac[18], iface_mac[18];
    FILE *fp;
    char iface_name[20], output_dir[256], file_name[512];
    char tstamp[16], dev_mac[20];

    while (1) {
        opt = getopt_long(argc, argv, "o:", long_options, &option_index);

        if (opt == -1)
            break;
        switch (opt)
            {
            case 'o':
                strcpy(output_dir, optarg);
                break;
            default:
                return 1;
            }
    }

    if (nls == (struct nl_sock *)NULL) {
        printf("cannot allocate nls socket, exiting\n");
        exit(1);
    }
  
    nl_socket_set_buffer_size(nls, 8192, 8192);

    ret = genl_connect(nls);
    if (ret < 0) {
        printf("cannot connect netlink socket, exiting\n");
        exit(1);
    }
  
    int driver_id = genl_ctrl_resolve(nls, "nl80211");
    if (driver_id < 0) {
        printf("cannot resolve nl80211 driver, exiting\n");
        exit(1);
    }

    ifc_args.jarray = json_object_new_array();
    ret = nl_socket_modify_cb(nls, NL_CB_VALID, NL_CB_CUSTOM,
                              get_interface_cb, (void *)&ifc_args);
    if (ret < 0) {
        printf("cannot define request callback function, exiting\n");
        exit(1);
    }

    struct nl_msg *msg = nlmsg_alloc();
    if (msg == (struct nl_msg *)NULL) {
        printf("cannot allocate nl_msg, exiting\n");
        exit(1);
    }
  
    genlmsg_put(msg, 0, 0, driver_id, 0, NLM_F_DUMP,
                NL80211_CMD_GET_INTERFACE, 0);

    ret = nl_send_auto_complete(nls, msg);
    if (ret < 0) {
        printf("cannot send request, exiting\n");
        exit(1);
    }
    
    ret = nl_recvmsgs_default(nls);
    if (ret < 0) {
        printf("error returned while processing data %d", ret);
    }

    /*
     * for now output the main json structure anyway
     */
    json_object *jinterface = json_object_new_object();
    json_object_object_add(jinterface, "Interface", ifc_args.jarray);
    json_object *jwifi = json_object_new_object();
    json_object_object_add(jwifi, "Wifi", jinterface);
    json_object *jdevice = json_object_new_object();
    json_object_object_add(jdevice, "Device", jwifi);

    time_t t = 0;
    time(&t);
    struct tm *tstruct = gmtime(&t);
    strftime(tstamp, 16, "%Y%m%d%H%M%S", tstruct);
    get_interface_mac("br-wan", dev_mac);
  
    sprintf(file_name, "%s/%s_interface_%s00.json", output_dir, dev_mac, tstamp);
    fp = fopen(file_name, "wb");
    if (!fp) {
        fprintf(stderr, "Cant open output file: %s", file_name);
        return 1;
    }
  
    fprintf(fp, "%s\n", json_object_to_json_string(jdevice));
    fclose(fp);
  

    return 0;

 nla_put_failure:
    return 1;
}

