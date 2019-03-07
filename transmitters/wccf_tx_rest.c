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

#include "wccf_tx_rest.h"

int main(int argc, char *argv[]) {
    static struct option long_options[] = {
        {"input-path", required_argument, 0, 'i'},
        {"host",       required_argument, 0, 'h'},
        {"port",       required_argument, 0, 'p'},
        {0,0,0,0}
    };
    int option_index = 0, opt = 0;
    CURL *curl;
    CURLcode res = 1;
    FILE * fd;
    struct stat file_info;
    char filename[256], full_filename[256];
    char input_path[256], rest_host[64], rest_port[8];
    char uri[128];
    char host_mac[18];
    long file_size = 0;
    char *data = NULL;

    input_path[0] = 0;
    rest_host[0] = 0;
    rest_port[0] = 0;
  
    while (1) {
        opt = getopt_long(argc, argv, "i:h:p:", long_options, &option_index);

        if (opt == -1)
            break;
        switch (opt) {
        case 'i':
            strcpy(input_path, optarg);
            break;
        case 'h':
            strcpy(rest_host, optarg);
            break;
        case 'p':
            strcpy(rest_port, optarg);
            break;
        default:
            return 1;
        }
    }

    if (input_path[0] == 0) {
        fprintf(stderr, "error, --input-path parameter is required, exiting\n");
        return 1;
    }
    else if (rest_host[0] == 0) {
        fprintf(stderr, "error, --host parameter is required, exiting\n");
        return 1;
    }
    else if (rest_port[0] == 0) {
        fprintf(stdout, "info, --port parameter defaulting to 80\n");
        strcpy(rest_port, "80");
    }

    curl_global_init(CURL_GLOBAL_ALL);

    while (1) {
        DIR *dir = NULL;
        struct dirent *ent = NULL;
   
        if (dir == NULL) {
            if ((dir = opendir(input_path)) == NULL) {
                fprintf(stderr, "error opening input directory: %s\n",
                        strerror(errno));
                return 1;
            }
        }
    
        filename[0] = 0;

        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type != DT_REG)
                continue;
            strcpy(filename, ent->d_name);
            break;
        }

        if (dir != NULL) {
            closedir(dir);
            dir = NULL;
        }

        if (filename[0] == 0) {
            sleep(1);
            continue;
        }

        res = 1;

        sprintf(full_filename, "%s/%s", input_path, filename);
    
        memset(host_mac, 0, 18);
        strncpy(host_mac, filename, 17);

        sprintf(uri, "http://%s:%s/wccf/rec/%s?fn=%s",
                rest_host, rest_port, host_mac, filename);
  
        stat(full_filename, &file_info);
        file_size = file_info.st_size;
        
        fd = fopen(full_filename, "rb");
        if (!fd) {
            fprintf(stderr, "error opening %s %s, skipping\n",
                    full_filename, strerror(errno));
            sleep(1);
            continue;
        }

        data = (char *)malloc(sizeof(char)*file_size);
        fread(data, sizeof(char)*file_size, 1, fd);
        
        curl = curl_easy_init();
        if (curl) {
            struct curl_slist *headers = NULL;
            headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
#if 0
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_URL, uri);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, file_size);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            res = curl_easy_perform(curl);
            if(res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));
                sleep(1);
            }
        }
    
        curl_easy_cleanup(curl);
        fclose(fd);
        free(data);
        
        if (res == CURLE_OK)
            unlink(full_filename);
    }

    curl_global_cleanup();
    
    return 0;
}

