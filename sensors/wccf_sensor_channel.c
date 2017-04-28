/*
  #
  # Copyright (c) 2017 Applied Broadband, Inc., and
  #                    Cable Television Laboratories, Inc. ("CableLabs")
  #
*/
#include "wccf_sensor_channel.h"

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
    json_object *jarray = ((struct survey_args *)arg)->jarray;
    char *if_mac_address = ((struct survey_args *)arg)->if_mac_address;
    struct nlattr *tb[NL80211_ATTR_MAX + 1];
    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *sinfo[NL80211_SURVEY_INFO_MAX + 1];
    char device[20];

    static struct nla_policy survey_policy[NL80211_SURVEY_INFO_MAX + 1] = {
        [NL80211_SURVEY_INFO_FREQUENCY] = { .type = NLA_U32 },
        [NL80211_SURVEY_INFO_NOISE] = { .type = NLA_U8 },
    };

    nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
              genlmsg_attrlen(gnlh, 0), NULL);

    if (tb[NL80211_ATTR_VHT_CAPABILITY]) {
        printf("found NL80211_ATTR_VHT_CAPABILITY\n");
    }
    if (tb[NL80211_ATTR_HT_CAPABILITY]) {
        printf("found NL80211_ATTR_HT_CAPABILITY\n");
    }
    
    json_object *jdev = json_object_new_object();

    /*
     * look up device name using interface index
     */
    if_indextoname(nla_get_u32(tb[NL80211_ATTR_IFINDEX]), device);
   
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
    json_object *jifindex = json_object_new_int(nla_get_u32(tb[NL80211_ATTR_IFINDEX]));
    json_object_object_add(jdev, "APIfIndex", jifindex);

    /*
     * current c-epoch of data sample
     */
    time_t current_time = time(0);

    json_object *jctime = json_object_new_int(current_time);
    json_object_object_add(jdev, "CurrentUTCTime", jctime);
  
    if (!tb[NL80211_ATTR_SURVEY_INFO]) {
        fprintf(stderr, "survey channel data missing!\n");
        return NL_SKIP;
    }

    if (nla_parse_nested(sinfo, NL80211_SURVEY_INFO_MAX,
                         tb[NL80211_ATTR_SURVEY_INFO],
                         survey_policy)) {
        json_object_array_add(jarray, jdev);
        return NL_SKIP;
    }

    if (sinfo[NL80211_SURVEY_INFO_FREQUENCY]) {
        int freq = (int)nla_get_u32(sinfo[NL80211_SURVEY_INFO_FREQUENCY]);
        int channel = 0;

        json_object *jfreq = json_object_new_int(freq);
        json_object_object_add(jdev, "Frequency", jfreq);
        /*
         * TODO: fix
         * couldn't get convenient access to the driver level conversion
         * logic so we duplicate it here (in cfg80211.h)
         */
        if (freq == 2484)
            channel = 14;
        else if (freq < 2484)
            channel = (freq - 2407) / 5;
        else if (freq >= 4910 && freq <= 4980)
            channel = (freq - 4000) / 5;
        else if (freq <= 45000)
            channel = (freq - 5000) / 5;
        else if (freq >= 58320 && freq <= 64800)
            channel = (freq - 56160) / 2160;

        json_object *jchan = json_object_new_int(channel);
        json_object_object_add(jdev, "ChannelNumber", jchan);                                               
    }
    if (tb[NL80211_ATTR_WIPHY_BANDS]) {
        printf("Have NL80211_ATTR_WIPHY_BANDS\n");
    }
    if (sinfo[NL80211_SURVEY_INFO_NOISE]) {
        int8_t noise = (int8_t)nla_get_u8(sinfo[NL80211_SURVEY_INFO_NOISE]);
        json_object *jnoise = json_object_new_int(noise);
        json_object_object_add(jdev, "Noise", jnoise);
    }
    // TODO fix issue with unsigned (using strings?)
    if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME]) {
        json_object *jactivetime =
            json_object_new_int64((long long)nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME]));
        json_object_object_add(jdev, "ChannelActiveTime", jactivetime);
    }
    if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_BUSY]) {
        json_object *jbusytime =
            json_object_new_int64((long long)nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_BUSY]));
        json_object_object_add(jdev, "ChannelBusyTime", jbusytime);
    }
    if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_EXT_BUSY]) {
        json_object *jbusyexttime =
            json_object_new_int64((long long)nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_EXT_BUSY]));
        json_object_object_add(jdev, "ChannelBusyExtTime", jbusyexttime);
    }
    if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_RX]) {
        json_object *jreceivetime =
            json_object_new_int64((long long)nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_RX]));
        json_object_object_add(jdev, "ChannelReceiveTime", jreceivetime);
    }
    if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_TX]) {
        json_object *jtransmittime =
            json_object_new_int64((long long)nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_TX]));
        json_object_object_add(jdev, "ChannelTransmitTime", jtransmittime);
    }
  
    json_object *jinuse = NULL;
    if (sinfo[NL80211_SURVEY_INFO_IN_USE]) {
        jinuse = json_object_new_int(1);
    }
    else {
        jinuse = json_object_new_int(0);
    }
  
    json_object_object_add(jdev, "InUse", jinuse);

    json_object_array_add(jarray, jdev);
  
    return NL_SKIP;
    //return 0;
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
    struct survey_args svy_args;
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

    svy_args.jarray = json_object_new_array();
    ret = nl_socket_modify_cb(nls, NL_CB_VALID, NL_CB_CUSTOM,
                              get_survey_cb, (void *)&svy_args);
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
  
    svy_args.if_mac_address = iface_mac;
  
    int if_index = if_nametoindex(iface_name);
    if (if_index == 0) {
        printf("cannot find index for %s, exiting\n", iface_name);
        exit(1);
    }

    genlmsg_put(msg, 0, 0, driver_id, 0, 773,
                NL80211_CMD_GET_SURVEY, 0);

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
    json_object_object_add(jstation, "Channel", svy_args.jarray);
    json_object *jwifi = json_object_new_object();
    json_object_object_add(jwifi, "Wifi", jstation);
    json_object *jdevice = json_object_new_object();
    json_object_object_add(jdevice, "Device", jwifi);

    time_t t = 0;
    time(&t);
    struct tm *tstruct = gmtime(&t);
    strftime(tstamp, 16, "%Y%m%d%H%M%S", tstruct);
    get_interface_mac("br-wan", dev_mac);
  
    sprintf(file_name, "%s/%s_channel-%s_%s00.json", output_dir,
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

