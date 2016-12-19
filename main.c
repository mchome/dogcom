#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "configparse.h"
#include "auth.h"

#ifndef WIN32
#include <linux/limits.h>
#include "daemon.h"
#endif

#define VERSION "1.2.1"

void print_help(int exval);

int main(int argc, char *argv[]) {
    if (argc == 1) {
        print_help(1);
    }

    char *file_path;

    while (1) {
        static const struct option long_options[] = {
            { "mode", required_argument, 0, 'm' },
            { "conf", required_argument, 0, 'c' },
            { "log", required_argument, 0, 'l' },
#ifndef WIN32
            { "daemon", no_argument, 0, 'd' },
#endif
            { "verbose", no_argument, 0, 'v' },
            { "help", no_argument, 0, 'h' },
            { 0, 0, 0, 0 }
        };

        int c;
        int option_index = 0;
#ifdef WIN32
        c = getopt_long(argc, argv, "m:c:l:vh", long_options, &option_index);
#else
        c = getopt_long(argc, argv, "m:c:l:dvh", long_options, &option_index);
#endif
        
        if (c == -1) {
            break;
        }
        switch (c) {
            case 'm':
                if (strcmp(optarg, "dhcp") == 0) {
                    mode = strdup(optarg);
                } else if (strcmp(optarg, "pppoe") == 0) {
                    mode = strdup(optarg);
                } else {
                    printf("unknown mode\n");
                    exit(1);
                }
                break;
            case 'c':
                if (mode != NULL) {
#ifdef WIN32
                    file_path = optarg;
#else
                    char path_c[PATH_MAX];
                    realpath(optarg, path_c);
                    file_path = strdup(path_c);
#endif
                }
                break;
            case 'l':
                if (mode != NULL) {
#ifdef WIN32
                    log_path = optarg;
#else
                    char path_l[PATH_MAX];
                    realpath(optarg, path_l);
                    log_path = strdup(path_l);
#endif
                    logging_flag = 1;
                }
                break;
#ifndef WIN32
            case 'd':
                daemon_flag = 1;
                break;
#endif
            case 'v':
                verbose_flag = 1;
                break;
            case 'h':
                print_help(0);
                break;
            case '?':
                print_help(1);
                break;
            default:
                break;
        }
    }

    if (mode != NULL && file_path != NULL) {
#ifndef WIN32
        if (daemon_flag) {
            daemonise();
        }
#endif
        if (!config_parse(file_path, mode)) {
            dogcom(5, mode);
        } else {
            return 1;
        }
    } else {
        printf("Need more options!\n\n");
        return 1;
    }
    return 0;
}

void print_help(int exval) {
    printf("\nDrcom-generic implementation in C.\n");
    printf("Version: %s\n\n", VERSION);

    printf("Usage:\n");
    printf("\tdogcom -m <dhcp/pppoe> -c <FILEPATH> [options <argument>]...\n\n");

    printf("Options:\n");
    printf("\t--mode <dhcp/pppoe>, -m <dhcp/pppoe>  set your dogcom mode \n");
    printf("\t--conf <FILEPATH>, -c <FILEPATH>      import configuration file\n");
    printf("\t--log <LOGPATH>, -l <LOGPATH>         specify log file\n");
#ifndef WIN32
    printf("\t--daemon, -d                          set daemon flag\n");
#endif
    printf("\t--verbose, -v                         set verbose flag\n");
    printf("\t--help, -h                            display this help\n\n");
    exit(exval);
}