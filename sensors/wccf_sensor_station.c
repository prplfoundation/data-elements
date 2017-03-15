/*
  #
  # Copyright (c) 2017 Applied Broadband, Inc., and
  #                    Cable Television Laboratories, Inc. ("CableLabs")
  #
*/
#include "wccf_sensor_station.h"

static int get_interface_cb(struct nl_msg *msg, void* arg)
{
    return 0;
}

/*
 * Callback routine to handle NL80211 responses to a NL80211_CMD_GET_STATIONS
 * request.
 */
static int get_station_cb(struct nl_msg *msg, void* arg)
{
    char mac_str[20], device[10];
    unsigned char mdat[6];
    json_object *jarray = ((struct station_args *)arg)->jarray;
    char *if_mac_address = ((struct station_args *)arg)->if_mac_address;
    struct nlmsghdr* hdr = nlmsg_hdr(msg);
    struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];
    struct nlattr *sinfo[NL80211_STA_INFO_MAX + 1];
    struct genlmsghdr *gnlhdr = (struct genlmsghdr *)nlmsg_data(hdr);
    static struct nla_policy stats_policy[NL80211_STA_INFO_MAX + 1] = {
        [NL80211_STA_INFO_INACTIVE_TIME] = { .type = NLA_U32 },
        [NL80211_STA_INFO_RX_BYTES] = { .type = NLA_U32 },
        [NL80211_STA_INFO_TX_BYTES] = { .type = NLA_U32 },
        [NL80211_STA_INFO_RX_PACKETS] = { .type = NLA_U32 },
        [NL80211_STA_INFO_TX_PACKETS] = { .type = NLA_U32 },
        [NL80211_STA_INFO_SIGNAL] = { .type = NLA_U8 },
        [NL80211_STA_INFO_T_OFFSET] = { .type = NLA_U64 },
        [NL80211_STA_INFO_TX_BITRATE] = { .type = NLA_NESTED },
        [NL80211_STA_INFO_RX_BITRATE] = { .type = NLA_NESTED },
        [NL80211_STA_INFO_LLID] = { .type = NLA_U16 },
        [NL80211_STA_INFO_PLID] = { .type = NLA_U16 },
        [NL80211_STA_INFO_PLINK_STATE] = { .type = NLA_U8 },
        [NL80211_STA_INFO_TX_RETRIES] = { .type = NLA_U32 },
        [NL80211_STA_INFO_TX_FAILED] = { .type = NLA_U32 },
        [NL80211_STA_INFO_STA_FLAGS] =
        { .minlen = sizeof(struct nl80211_sta_flag_update) },
        [NL80211_STA_INFO_LOCAL_PM] = { .type = NLA_U32},
        [NL80211_STA_INFO_PEER_PM] = { .type = NLA_U32},
        [NL80211_STA_INFO_NONPEER_PM] = { .type = NLA_U32},
        [NL80211_STA_INFO_CHAIN_SIGNAL] = { .type = NLA_NESTED },
        [NL80211_STA_INFO_CHAIN_SIGNAL_AVG] = { .type = NLA_NESTED },
    };
    struct nlattr *rinfo[NL80211_RATE_INFO_MAX + 1];
    static struct nla_policy rate_policy[NL80211_RATE_INFO_MAX + 1] = {
        [NL80211_RATE_INFO_BITRATE] = { .type = NLA_U16 },
        [NL80211_RATE_INFO_BITRATE32] = { .type = NLA_U32 },
        [NL80211_RATE_INFO_MCS] = { .type = NLA_U8 },
        [NL80211_RATE_INFO_40_MHZ_WIDTH] = { .type = NLA_FLAG },
        [NL80211_RATE_INFO_SHORT_GI] = { .type = NLA_FLAG },
    };

    nla_parse(tb_msg, NL80211_ATTR_MAX, genlmsg_attrdata(gnlhdr, 0),
              genlmsg_attrlen(gnlhdr, 0), NULL);

    json_object *jdev = json_object_new_object();

    /*
     * look up device name using interface index
     */
    if_indextoname(nla_get_u32(tb_msg[NL80211_ATTR_IFINDEX]), device);

    json_object *jdevname = json_object_new_string(device);
    json_object_object_add(jdev, "APName", jdevname);

    /*
     * repeat AP Interface MAC address
     */
    json_object *japmac = json_object_new_string(if_mac_address);
    json_object_object_add(jdev, "APMACAddress", japmac);

    /*
     * output interface index as numeric
     */
    json_object *jifindex = json_object_new_int(nla_get_u32(tb_msg[NL80211_ATTR_IFINDEX]));
    json_object_object_add(jdev, "APIfIndex", jifindex);
  
    /*
     * current c-epoch of data sample
     */
    time_t current_time = time(0);

    json_object *jctime = json_object_new_int(current_time);
    json_object_object_add(jdev, "CurrentUTCTime", jctime);
  
    /*
     * format MAC address for output
     */
    memset(mac_str, 0, sizeof(mac_str));
    memcpy(mdat, nla_data(tb_msg[NL80211_ATTR_MAC]), sizeof(mdat));
    sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x",
            mdat[0],mdat[1],mdat[2],mdat[3],mdat[4],mdat[5]);
  
    json_object *jmac = json_object_new_string(mac_str);
    json_object_object_add(jdev, "MACAddress", jmac);

    if (!tb_msg[NL80211_ATTR_STA_INFO]) {
        //fprintf(stderr, "sta stats missing!\n");
        json_object_array_add(jarray, jdev);
        return NL_SKIP;
    }

    /*
     * parse the station-specific data 
     */
    if (nla_parse_nested(sinfo, NL80211_STA_INFO_MAX,
                         tb_msg[NL80211_ATTR_STA_INFO],
                         stats_policy)) {
        json_object_array_add(jarray, jdev);
        return NL_SKIP;
    }

    if (sinfo[NL80211_STA_INFO_INACTIVE_TIME]) {
        json_object *jinactive =
            json_object_new_int(nla_get_u32(sinfo[NL80211_STA_INFO_INACTIVE_TIME]));
        json_object_object_add(jdev, "InactiveTime", jinactive);
    }
    if (sinfo[NL80211_STA_INFO_RX_BYTES64]) {
        json_object *jrxbytes =
            json_object_new_int((unsigned long long)nla_get_u64(sinfo[NL80211_STA_INFO_RX_BYTES64]));
        json_object_object_add(jdev, "RxBytes", jrxbytes);
    }
    else if (sinfo[NL80211_STA_INFO_RX_BYTES]) {
        json_object *jrxbytes =
            json_object_new_int(nla_get_u32(sinfo[NL80211_STA_INFO_RX_BYTES]));
        json_object_object_add(jdev, "RxBytes", jrxbytes);
    }
    if (sinfo[NL80211_STA_INFO_RX_PACKETS]) {
        json_object *jrxpackets =
            json_object_new_int(nla_get_u32(sinfo[NL80211_STA_INFO_RX_PACKETS]));
        json_object_object_add(jdev, "RxPackets", jrxpackets);
    }
    if (sinfo[NL80211_STA_INFO_TX_BYTES64]) {
        json_object *jtxbytes =
            json_object_new_int((unsigned long long)nla_get_u64(sinfo[NL80211_STA_INFO_TX_BYTES64]));
        json_object_object_add(jdev, "TxBytes", jtxbytes);
    }
    else if (sinfo[NL80211_STA_INFO_TX_BYTES]) {
        json_object *jtxbytes =
            json_object_new_int(nla_get_u32(sinfo[NL80211_STA_INFO_TX_BYTES]));
        json_object_object_add(jdev, "TxBytes", jtxbytes);
    }
    if (sinfo[NL80211_STA_INFO_TX_PACKETS]) {
        json_object *jtxpackets =
            json_object_new_int(nla_get_u32(sinfo[NL80211_STA_INFO_TX_PACKETS]));
        json_object_object_add(jdev, "TxPackets", jtxpackets);
    }
    if (sinfo[NL80211_STA_INFO_TX_RETRIES]) {
        json_object *jtxretries =
            json_object_new_int(nla_get_u32(sinfo[NL80211_STA_INFO_TX_RETRIES]));
        json_object_object_add(jdev, "TxRetries", jtxretries);
    }
    if (sinfo[NL80211_STA_INFO_TX_FAILED]) {
        json_object *jtxfailed =
            json_object_new_int(nla_get_u32(sinfo[NL80211_STA_INFO_TX_FAILED]));
        json_object_object_add(jdev, "TxFailed", jtxfailed);
    }

    if (sinfo[NL80211_STA_INFO_SIGNAL]) {
        int8_t sig = (int8_t)nla_get_u8(sinfo[NL80211_STA_INFO_SIGNAL]);
        json_object *jsignal = json_object_new_int(sig);
        json_object_object_add(jdev, "Signal", jsignal);
    }

    if (sinfo[NL80211_STA_INFO_SIGNAL_AVG]) {
        int8_t sigavg = (int8_t)nla_get_u8(sinfo[NL80211_STA_INFO_SIGNAL_AVG]);
        json_object *jsignalavg = json_object_new_int(sigavg);
        json_object_object_add(jdev, "SignalAvg", jsignalavg);
    }

    // TODO Collapse logic into function for the bitrate calculations
    if (sinfo[NL80211_STA_INFO_TX_BITRATE]) {
        int rate = 0;
        if (!nla_parse_nested(rinfo, NL80211_RATE_INFO_MAX,
                              sinfo[NL80211_STA_INFO_TX_BITRATE], rate_policy)) {
            if (rinfo[NL80211_RATE_INFO_BITRATE32])
                rate = nla_get_u32(rinfo[NL80211_RATE_INFO_BITRATE32]);
            else if (rinfo[NL80211_RATE_INFO_BITRATE])
                rate = nla_get_u16(rinfo[NL80211_RATE_INFO_BITRATE]);
            json_object *jtxbitrate =
                json_object_new_double(((double)rate/10.0)+((double)(rate%10)/10.0));
            json_object_object_add(jdev, "TxBitrate", jtxbitrate);
            if (rinfo[NL80211_RATE_INFO_MCS]) {
                uint8_t mcs = nla_get_u8(rinfo[NL80211_RATE_INFO_MCS]);
                json_object *jtxmcs = json_object_new_int(mcs);
                json_object_object_add(jdev, "HtTxMCS", jtxmcs);
            }
            else if (rinfo[NL80211_RATE_INFO_VHT_MCS]) {
                uint8_t mcs = nla_get_u8(rinfo[NL80211_RATE_INFO_VHT_MCS]);
                json_object *jtxmcs = json_object_new_int(mcs);
                json_object_object_add(jdev, "VhtTxMCS", jtxmcs);
            }
            if (rinfo[NL80211_RATE_INFO_VHT_NSS]) {
                uint8_t nss = nla_get_u8(rinfo[NL80211_RATE_INFO_VHT_NSS]);
                json_object *jtxnss = json_object_new_int(nss);
                json_object_object_add(jdev, "VhtTxNSS", jtxnss);
            }
            if (rinfo[NL80211_RATE_INFO_SHORT_GI]) {
                json_object *jtxsgi = json_object_new_string("true");
                json_object_object_add(jdev, "VhtTxShortGI", jtxsgi);
            }
        }
    }

    if (sinfo[NL80211_STA_INFO_RX_BITRATE]) {
        int rate = 0;
        if (!nla_parse_nested(rinfo, NL80211_RATE_INFO_MAX,
                              sinfo[NL80211_STA_INFO_RX_BITRATE], rate_policy)) {
            if (rinfo[NL80211_RATE_INFO_BITRATE32])
                rate = nla_get_u32(rinfo[NL80211_RATE_INFO_BITRATE32]);
            else if (rinfo[NL80211_RATE_INFO_BITRATE])
                rate = nla_get_u16(rinfo[NL80211_RATE_INFO_BITRATE]);
            json_object *jrxbitrate =
                json_object_new_double(((double)(rate/10))+((double)((rate%10)/10.0)));
            json_object_object_add(jdev, "RxBitrate", jrxbitrate);
            if (rinfo[NL80211_RATE_INFO_MCS]) {
                uint8_t mcs = nla_get_u8(rinfo[NL80211_RATE_INFO_MCS]);
                json_object *jrxmcs = json_object_new_int(mcs);
                json_object_object_add(jdev, "HtRxMCS", jrxmcs);
            }
            else if (rinfo[NL80211_RATE_INFO_VHT_MCS]) {
                uint8_t mcs = nla_get_u8(rinfo[NL80211_RATE_INFO_VHT_MCS]);
                json_object *jrxmcs = json_object_new_int(mcs);
                json_object_object_add(jdev, "VhtRxMCS", jrxmcs);
            }
            if (rinfo[NL80211_RATE_INFO_VHT_NSS]) {
                uint8_t nss = nla_get_u8(rinfo[NL80211_RATE_INFO_VHT_NSS]);
                json_object *jrxnss = json_object_new_int(nss);
                json_object_object_add(jdev, "VhtRxNSS", jrxnss);
            }
            if (rinfo[NL80211_RATE_INFO_SHORT_GI]) {
                json_object *jrxsgi = json_object_new_string("true");
                json_object_object_add(jdev, "VhtRxShortGI", jrxsgi);
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
        {"interface-name", required_argument, 0, 'i'},
        {"output-dir",     required_argument, 0, 'o'},
        {0,0,0,0}
    };
    int option_index = 0, opt = 0;
    int ret = 0;
    int flags = 773; //OK ?
    struct station_args sta_args;
    struct nl_sock *nls = nl_socket_alloc();
    char station_mac[18], iface_mac[18];
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

    sta_args.jarray = json_object_new_array();
    ret = nl_socket_modify_cb(nls, NL_CB_VALID, NL_CB_CUSTOM,
                              get_station_cb, (void *)&sta_args);
    if (ret < 0) {
        printf("cannot define request callback function, exiting\n");
        exit(1);
    }

    struct nl_msg *msg = nlmsg_alloc();
    if (msg == (struct nl_msg *)NULL) {
        printf("cannot allocate nl_msg, exiting\n");
        exit(1);
    }
  
    memset(iface_mac, 0, 18);
    ret = get_interface_mac(iface_name, iface_mac);
  
    sta_args.if_mac_address = iface_mac;
  
    int if_index = if_nametoindex(iface_name);
    if (if_index == 0) {
        printf("cannot find index for %s, exiting\n", iface_name);
        exit(1);
    }

    genlmsg_put(msg, 0, 0, driver_id, 0, flags,
                NL80211_CMD_GET_STATION, 0);

    NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, if_index);

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
    json_object *jstation = json_object_new_object();
    json_object_object_add(jstation, "Station", sta_args.jarray);
    json_object *jwifi = json_object_new_object();
    json_object_object_add(jwifi, "Wifi", jstation);
    json_object *jdevice = json_object_new_object();
    json_object_object_add(jdevice, "Device", jwifi);

    time_t t = 0;
    time(&t);
    struct tm *tstruct = gmtime(&t);
    strftime(tstamp, 16, "%Y%m%d%H%M%S", tstruct);
    get_interface_mac("br-wan", dev_mac);
  
    sprintf(file_name, "%s/%s_station-%s_%s00.json", output_dir, dev_mac, iface_name, tstamp);
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

