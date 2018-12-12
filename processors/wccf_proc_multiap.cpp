/*
  #
  # Copyright (c) 2017 Applied Broadband, Inc., and
  #                    Cable Television Laboratories, Inc. ("CableLabs")
  #
*/
#include "wccf_proc_multiap.h"

int main(int argc, char *argv[]) {
    static struct option long_options[] = {
        {"input-path", required_argument, 0, 'i'},
        {"output-path",required_argument, 0, 'o'},
        {"collection-interval", required_argument, 0, 'c'},
        {"scan-offset", required_argument, 0, 's'},
        {0,0,0,0}
    };
    int option_index = 0, opt = 0;
    char input_path[256], output_path[256];
    unsigned collection_interval = 900, scan_offset = 60;

    input_path[0] = 0;
    output_path[0] = 0;
    
    while (1) {
        opt = getopt_long(argc, argv, "i:o:c:s:", long_options, &option_index);

        if (opt == -1)
            break;
        switch (opt) {
        case 'i':
            strcpy(input_path, optarg);
            break;
        case 'o':
            strcpy(output_path, optarg);
            break;
        case 'c':
            collection_interval = stoul(optarg);
            break;
        case 's':
            scan_offset = stoul(optarg);
            break;
        default:
            return 1;
        }
    }
    
    if (input_path[0] == 0) {
        fprintf(stderr, "error, --input-path parameter is required, exiting\n");
        return 1;
    }
    else if (output_path[0] == 0) {
        fprintf(stderr, "error, --output-path parameter is required, exiting\n");
        return 1;
    }

    string inputPath = string(input_path);
    string outputPath = string(output_path);
    while (1) {
        WCCFProcMultiAP ma = WCCFProcMultiAP(inputPath, outputPath);
        ma.processDataFiles();
        ma.generateNetworkReport(collection_interval*1000, scan_offset);
        ma.purgeDataFiles();
    }

    return 0;
}

