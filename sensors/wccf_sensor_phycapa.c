/*
  #
  # Copyright (c) 2017 Applied Broadband, Inc., and
  #                    Cable Television Laboratories, Inc. ("CableLabs")
  #
*/
#include "wccf_sensor_phycapa.h"

static int get_interface_cb(struct nl_msg *msg, void* arg)
{
    return 0;
}

/*
 * Callback routine to handle NL80211 responses to a NL80211_CMD_GET_SURVEY
 * request.
 */
static int get_survey_cb(struct nl_msg *msg, void* arg)
{
    json_object *jarray = ((struct phycapa_args *)arg)->jarray;
    char *if_mac_address = ((struct phycapa_args *)arg)->if_mac_address;
    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
    struct phycapa_args *ctx = arg;
    struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];
    struct nlattr *tb_band[NL80211_BAND_ATTR_MAX + 1];
    struct nlattr *tb_freq[NL80211_FREQUENCY_ATTR_MAX + 1];
    struct nlattr *nl_band;
    struct nlattr *nl_freq;
    int rem_band, rem_freq;
    char device[20];
    
    nla_parse(tb_msg, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
              genlmsg_attrlen(gnlh, 0), NULL);

    if (tb_msg[NL80211_ATTR_WIPHY_BANDS]) {
        
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
        json_object *japmac = json_object_new_string(if_mac_address);
        json_object_object_add(jdev, "APMACAddress", japmac);

        /*
         * output interface index as numeric
         */
        if (tb_msg[NL80211_ATTR_IFINDEX]) {
            json_object *jifindex =
                json_object_new_int(nla_get_u32(tb_msg[NL80211_ATTR_IFINDEX]));
            json_object_object_add(jdev, "APIfIndex", jifindex);
        }
    
        /*
         * current c-epoch of data sample
         */
        time_t current_time = time(0);

        json_object *jctime = json_object_new_int(current_time);
        json_object_object_add(jdev, "CurrentUTCTime", jctime);
    
        nla_for_each_nested(nl_band,
                            tb_msg[NL80211_ATTR_WIPHY_BANDS],
                            rem_band) {
            if (ctx->last_band != nl_band->nla_type) {
                ctx->width_40 = false;
                ctx->width_80 = false;
                ctx->width_160 = false;
                ctx->last_band = nl_band->nla_type;
            }
            nla_parse(tb_band,
                      NL80211_BAND_ATTR_MAX,
                      nla_data(nl_band),
                      nla_len(nl_band), NULL);

            if (tb_band[NL80211_BAND_ATTR_HT_CAPA]) {
                __u16 cap = nla_get_u16(tb_band[NL80211_BAND_ATTR_HT_CAPA]);
                
                if (cap & BIT(1))
                    ctx->width_40 = true;
            }
            if (tb_band[NL80211_BAND_ATTR_VHT_CAPA]) {
                __u32 capa;

                ctx->width_80 = true;

                capa = nla_get_u32(tb_band[NL80211_BAND_ATTR_VHT_CAPA]);
                switch ((capa >> 2) & 3) {
                case 2:
                case 1:
                    ctx->width_160 = true;
                    break;
                }
            }
            
            if (tb_band[NL80211_BAND_ATTR_FREQS]) {

                json_object *jcapas = json_object_new_array();
                
                nla_for_each_nested(nl_freq,
                                    tb_band[NL80211_BAND_ATTR_FREQS],
                                    rem_freq) {
                    uint32_t freq;

                    nla_parse(tb_freq, NL80211_FREQUENCY_ATTR_MAX,
                              nla_data(nl_freq),
                              nla_len(nl_freq), NULL);

                    if (!tb_freq[NL80211_FREQUENCY_ATTR_FREQ])
                        continue;

                    if (tb_freq[NL80211_FREQUENCY_ATTR_DISABLED]) {
                        continue;
                    }

                    json_object *jcapa = json_object_new_object();
                    
                    freq = nla_get_u32(tb_freq[NL80211_FREQUENCY_ATTR_FREQ]);
                    
                    json_object *jfreq = json_object_new_int(freq);
                    json_object_object_add(jcapa, "Frequency", jfreq);

                    int chan = 0;
                    
                    if (freq == 2484)
                        chan = 14;
                    else if (freq < 2484)
                        chan = (freq - 2407) / 5;
                    else if (freq >= 4910 && freq <= 4980)
                        chan = (freq - 4000) / 5;
                    else if (freq <= 45000) /* DMG band lower limit */
                        chan = (freq - 5000) / 5;
                    else if (freq >= 58320 && freq <= 64800)
                        chan = (freq - 56160) / 2160;
                    
                    json_object *jchan = json_object_new_int(chan);
                    json_object_object_add(jcapa, "ChannelNumber", jchan);
                    
                    if (tb_freq[NL80211_FREQUENCY_ATTR_MAX_TX_POWER]) {
                        double power =  0.01 * nla_get_u32(tb_freq[NL80211_FREQUENCY_ATTR_MAX_TX_POWER]);
                        json_object *jpow = json_object_new_double(power);
                        json_object_object_add(jcapa, "MaxTxPower", jpow);
                    }
                    
                    json_object *jwidth = json_object_new_array();
                     
                    if (!tb_freq[NL80211_FREQUENCY_ATTR_NO_20MHZ])
                        json_object_array_add(jwidth, json_object_new_string("20MHz"));
                    if (ctx->width_40 && !tb_freq[NL80211_FREQUENCY_ATTR_NO_HT40_MINUS])
                        json_object_array_add(jwidth, json_object_new_string("HT40-"));
                    if (ctx->width_40 && !tb_freq[NL80211_FREQUENCY_ATTR_NO_HT40_PLUS])
                        json_object_array_add(jwidth, json_object_new_string("HT40+"));
                    if (ctx->width_80 && !tb_freq[NL80211_FREQUENCY_ATTR_NO_80MHZ])
                        json_object_array_add(jwidth, json_object_new_string("VHT80"));
                    if (ctx->width_160 && !tb_freq[NL80211_FREQUENCY_ATTR_NO_160MHZ])
                        json_object_array_add(jwidth, json_object_new_string("VHT160"));
                    
                    json_object_object_add(jcapa, "ChannelWidth", jwidth);

                    json_object_array_add(jcapas, jcapa);
                }
                json_object_object_add(jdev, "Capabilities", jcapas);
            
                if (tb_band[NL80211_BAND_ATTR_VHT_CAPA] &&
                    tb_band[NL80211_BAND_ATTR_VHT_MCS_SET]) {
                    unsigned tmp = 0;
                    unsigned char *mcs = nla_data(tb_band[NL80211_BAND_ATTR_VHT_MCS_SET]);

                    json_object *jrxarray = json_object_new_array();
                    
                    tmp = mcs[0] | (mcs[1] << 8);
                    for (int i = 1; i <= 8; i++) {
                        switch ((tmp >> ((i-1)*2) ) & 3) {
                        case 0:
                            {
                                json_object *jobj = json_object_new_object();
                                json_object_object_add(jobj, "NSS", json_object_new_int(i));
                                json_object *jrxmcs = json_object_new_array();
                                json_object_array_add(jrxmcs, json_object_new_int(0));
                                json_object_array_add(jrxmcs, json_object_new_int(1));
                                json_object_array_add(jrxmcs, json_object_new_int(2));
                                json_object_array_add(jrxmcs, json_object_new_int(3));
                                json_object_array_add(jrxmcs, json_object_new_int(4));
                                json_object_array_add(jrxmcs, json_object_new_int(5));
                                json_object_array_add(jrxmcs, json_object_new_int(6));
                                json_object_array_add(jrxmcs, json_object_new_int(7));
                                json_object_object_add(jobj, "MCS", jrxmcs);
                                json_object_array_add(jrxarray, jobj);
                            }
                            break;
                        case 1:
                            {
                                json_object *jobj = json_object_new_object();
                                json_object_object_add(jobj, "NSS", json_object_new_int(i));
                                json_object *jrxmcs = json_object_new_array();
                                json_object_array_add(jrxmcs, json_object_new_int(0));
                                json_object_array_add(jrxmcs, json_object_new_int(1));
                                json_object_array_add(jrxmcs, json_object_new_int(2));
                                json_object_array_add(jrxmcs, json_object_new_int(3));
                                json_object_array_add(jrxmcs, json_object_new_int(4));
                                json_object_array_add(jrxmcs, json_object_new_int(5));
                                json_object_array_add(jrxmcs, json_object_new_int(6));
                                json_object_array_add(jrxmcs, json_object_new_int(7));
                                json_object_array_add(jrxmcs, json_object_new_int(8));
                                json_object_object_add(jobj, "MCS", jrxmcs);
                                json_object_array_add(jrxarray, jobj);
                            }
                            break;
                        case 2:
                            {
                                json_object *jobj = json_object_new_object();
                                json_object_object_add(jobj, "NSS", json_object_new_int(i));
                                json_object *jrxmcs = json_object_new_array();
                                json_object_array_add(jrxmcs, json_object_new_int(0));
                                json_object_array_add(jrxmcs, json_object_new_int(1));
                                json_object_array_add(jrxmcs, json_object_new_int(2));
                                json_object_array_add(jrxmcs, json_object_new_int(3));
                                json_object_array_add(jrxmcs, json_object_new_int(4));
                                json_object_array_add(jrxmcs, json_object_new_int(5));
                                json_object_array_add(jrxmcs, json_object_new_int(6));
                                json_object_array_add(jrxmcs, json_object_new_int(7));
                                json_object_array_add(jrxmcs, json_object_new_int(8));
                                json_object_array_add(jrxmcs, json_object_new_int(9));
                                json_object_object_add(jobj, "MCS", jrxmcs);
                                json_object_array_add(jrxarray, jobj);
                            }
                            break;
                        case 3: break;
                        }
                    }
                    
                    json_object_object_add(jdev, "VHTRxMCSSet", jrxarray);
                    
                    json_object *jtxarray = json_object_new_array();

                    tmp = mcs[4] | (mcs[5] << 8);
                    for (int i = 1; i <= 8; i++) {
                        switch ((tmp >> ((i-1)*2) ) & 3) {
                        case 0:
                            {
                                json_object *jobj = json_object_new_object();
                                json_object_object_add(jobj, "NSS", json_object_new_int(i));
                                json_object *jtxmcs = json_object_new_array();
                                json_object_array_add(jtxmcs, json_object_new_int(0));
                                json_object_array_add(jtxmcs, json_object_new_int(1));
                                json_object_array_add(jtxmcs, json_object_new_int(2));
                                json_object_array_add(jtxmcs, json_object_new_int(3));
                                json_object_array_add(jtxmcs, json_object_new_int(4));
                                json_object_array_add(jtxmcs, json_object_new_int(5));
                                json_object_array_add(jtxmcs, json_object_new_int(6));
                                json_object_array_add(jtxmcs, json_object_new_int(7));
                                json_object_object_add(jobj, "MCS", jtxmcs);
                                json_object_array_add(jtxarray, jobj);
                            }
                            break;
                        case 1:
                            {
                                json_object *jobj = json_object_new_object();
                                json_object_object_add(jobj, "NSS", json_object_new_int(i));
                                json_object *jtxmcs = json_object_new_array();
                                json_object_array_add(jtxmcs, json_object_new_int(0));
                                json_object_array_add(jtxmcs, json_object_new_int(1));
                                json_object_array_add(jtxmcs, json_object_new_int(2));
                                json_object_array_add(jtxmcs, json_object_new_int(3));
                                json_object_array_add(jtxmcs, json_object_new_int(4));
                                json_object_array_add(jtxmcs, json_object_new_int(5));
                                json_object_array_add(jtxmcs, json_object_new_int(6));
                                json_object_array_add(jtxmcs, json_object_new_int(7));
                                json_object_array_add(jtxmcs, json_object_new_int(8));
                                json_object_object_add(jobj, "MCS", jtxmcs);
                                json_object_array_add(jtxarray, jobj);
                            }
                            break;
                        case 2:
                            {
                                json_object *jobj = json_object_new_object();
                                json_object_object_add(jobj, "NSS", json_object_new_int(i));
                                json_object *jtxmcs = json_object_new_array();
                                json_object_array_add(jtxmcs, json_object_new_int(0));
                                json_object_array_add(jtxmcs, json_object_new_int(1));
                                json_object_array_add(jtxmcs, json_object_new_int(2));
                                json_object_array_add(jtxmcs, json_object_new_int(3));
                                json_object_array_add(jtxmcs, json_object_new_int(4));
                                json_object_array_add(jtxmcs, json_object_new_int(5));
                                json_object_array_add(jtxmcs, json_object_new_int(6));
                                json_object_array_add(jtxmcs, json_object_new_int(7));
                                json_object_array_add(jtxmcs, json_object_new_int(8));
                                json_object_array_add(jtxmcs, json_object_new_int(9));
                                json_object_object_add(jobj, "MCS", jtxmcs);
                                json_object_array_add(jtxarray, jobj);
                            }
                            break;
                        case 3: break;
                        }
                    }
                    
                    json_object_object_add(jdev, "VHTTxMCSSet", jtxarray);

                }
                
                if (tb_band[NL80211_BAND_ATTR_VHT_CAPA]) {
                    unsigned capa = nla_get_u32(tb_band[NL80211_BAND_ATTR_VHT_CAPA]);
                    unsigned gival = (capa & 0x060) >> 5;
                    if (gival) {
                        json_object *jshortgi; 
                        switch (gival) {
                        case 1:
                            jshortgi = json_object_new_string("80MHz");
                            break;
                        case 2:
                        case 3:
                            jshortgi = json_object_new_string("160MHz");
                            break;
                        }
                        json_object_object_add(jdev, "VHTShortGI", jshortgi);
                    }
                }
                
                if (tb_band[NL80211_BAND_ATTR_HT_MCS_SET] &&
                    nla_len(tb_band[NL80211_BAND_ATTR_HT_MCS_SET]) == 16) {
                    json_object *jhtmcs = json_object_new_array();
                    {
                        int mcs_bit, prev_bit = -2, prev_cont = 0;
                        unsigned char *mcs = nla_data(tb_band[NL80211_BAND_ATTR_HT_MCS_SET]);

                        for (mcs_bit = 0; mcs_bit <= 76; mcs_bit++) {
                            unsigned int mcs_octet = mcs_bit/8;
                            unsigned int MCS_RATE_BIT = 1 << mcs_bit % 8;
                            bool mcs_rate_idx_set;

                            mcs_rate_idx_set = !!(mcs[mcs_octet] & MCS_RATE_BIT);

                            if (mcs_rate_idx_set)
                                json_object_array_add(jhtmcs, json_object_new_int(mcs_bit));
                        }
                    }

                    json_object_object_add(jdev, "HTMCS", jhtmcs);
                }
            }

        }
        json_object_array_add(jarray, jdev);
    }

    return NL_SKIP;
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
    struct phycapa_args pc_args;
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
        printf("cannot resolver nl80211 driver, exiting\n");
        exit(1);
    }

    pc_args.jarray = json_object_new_array();
    pc_args.last_band = -1;
    
    ret = nl_socket_modify_cb(nls, NL_CB_VALID, NL_CB_CUSTOM,
                              get_survey_cb, (void *)&pc_args);
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
  
    pc_args.if_mac_address = iface_mac;
  
    int if_index = if_nametoindex(iface_name);
    if (if_index == 0) {
        printf("cannot find index for %s, exiting\n", iface_name);
        exit(1);
    }

    genlmsg_put(msg, 0, 0, driver_id, 0, 773,
                NL80211_CMD_GET_WIPHY, 0);

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
    json_object_object_add(jstation, "PhyCapa", pc_args.jarray);
    json_object *jwifi = json_object_new_object();
    json_object_object_add(jwifi, "Wifi", jstation);
    json_object *jdevice = json_object_new_object();
    json_object_object_add(jdevice, "Device", jwifi);

    time_t t = 0;
    time(&t);
    struct tm *tstruct = gmtime(&t);
    strftime(tstamp, 16, "%Y%m%d%H%M%S", tstruct);
    get_interface_mac("br-wan", dev_mac);
  
    sprintf(file_name, "%s/%s_phycapa-%s_%s00.json", output_dir,
            dev_mac, iface_name, tstamp);
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

