/*
  #
  # Copyright (c) 2017 Applied Broadband, Inc., and
  #                    Cable Television Laboratories, Inc. ("CableLabs")
  #
*/
#include "wccf_proc_null.h"

int main(int argc, char *argv[]) {
    static struct option long_options[] = {
        {"input-path", required_argument, 0, 'i'},
        {"output-path",required_argument, 0, 'o'},
        {0,0,0,0}
    };
    int option_index = 0, opt = 0;
    char input_path[256], output_path[256];

    input_path[0] = 0;
    output_path[0] = 0;
    
    while (1) {
        opt = getopt_long(argc, argv, "i:o:", long_options, &option_index);

        if (opt == -1)
            break;
        switch (opt) {
        case 'i':
            strcpy(input_path, optarg);
            break;
        case 'o':
            strcpy(output_path, optarg);
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

    while (1) {
        DIR *dir = NULL;
        struct dirent *ent = NULL;
   
        if ((dir = opendir(input_path)) == NULL) {
            fprintf(stderr, "error opening input directory: %s\n",
                    strerror(errno));
            return 1;
        }

        while ((ent = readdir(dir)) != NULL) {
            char oldname[512], newname[512];
            int status;
            if (ent->d_type != DT_REG)
                continue;
            sprintf(oldname, "%s/%s", input_path, ent->d_name);
            sprintf(newname, "%s/%s", output_path, ent->d_name);
            status = rename(oldname, newname);
            if (status < 0) {
                fprintf(stderr, "error renaming file: %s\n\tto: %s\n \t%s\n",
                        strerror(errno));
            }
        }

        if (dir != NULL) {
            closedir(dir);
            dir = NULL;
        }

        sleep(1);
    }
    
    return 0;
}

