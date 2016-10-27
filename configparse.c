#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "configparse.h"

int verbose_flag = 0;
char *mode;
struct config drcom_config;

static int read_d_config(char *buf, int size);
static int read_p_config(char *buf, int size);

int config_parse(char *filepath, char *mode) {
    FILE *ptr_file;
    char buf[100];

    ptr_file = fopen(filepath, "r");
    if (!ptr_file) {
        printf("Failed to read config file.\n");
        exit(1);
    }

    while (fgets(buf, sizeof(buf), ptr_file)) {
        // printf("%s", buf);
        if (strcmp(mode, "dhcp") == 0) {
            read_d_config(buf, sizeof(buf));
        } else if (strcmp(mode, "pppoe") == 0) {
            read_p_config(buf, sizeof(buf));
        }
    }
    fclose(ptr_file);

    return 0;
}

static int read_d_config(char *buf, int size) {
    if (verbose_flag) {
        printf("%s", buf);
    }

    char *delim = " ='\r\n";
    char *delim2 = "\\x";
    char *key;
    char *value;
    if (strlen(key = strtok(buf, delim))) {
        value = strtok(NULL, delim);
    }

    if (strcmp(key, "server") == 0) {
        strcpy(drcom_config.server, value);
        printf("%s\n", drcom_config.server);
    } else if (strcmp(key, "username") == 0) {
        drcom_config.username = value;
        printf("%s\n", drcom_config.username);
    } else if (strcmp(key, "password") == 0) {
        drcom_config.password = value;
        printf("%s\n", drcom_config.password);
    } else if (strcmp(key, "CONTROLCHECKSTATUS") == 0) {
        value = strtok(value, delim2);
        sscanf(value, "%02x", &drcom_config.CONTROLCHECKSTATUS);
        printf("0x%02x\n", drcom_config.CONTROLCHECKSTATUS);
    } else if (strcmp(key, "ADAPTERNUM") == 0) {
        value = strtok(value, delim2);
        sscanf(value, "%02x", &drcom_config.ADAPTERNUM);
        printf("0x%02x\n", drcom_config.ADAPTERNUM);
    } else if (strcmp(key, "host_ip") == 0) {
        strcpy(drcom_config.host_ip, value);
        printf("%s\n", drcom_config.host_ip);
    } else if (strcmp(key, "IPDOG") == 0) {
        value = strtok(value, delim2);
        sscanf(value, "%02x", &drcom_config.IPDOG);
        printf("0x%02x\n", drcom_config.IPDOG);
    } else if (strcmp(key, "host_name") == 0) {
        drcom_config.host_name = value;
        printf("%s\n", drcom_config.host_name);
    } else if (strcmp(key, "PRIMARY_DNS") == 0) {
        strcpy(drcom_config.PRIMARY_DNS, value);
        printf("%s\n", drcom_config.PRIMARY_DNS);
    } else if (strcmp(key, "dhcp_server") == 0) {
        strcpy(drcom_config.dhcp_server, value);
        printf("%s\n", drcom_config.dhcp_server);
    } else if (strcmp(key, "AUTH_VERSION") == 0) {
        
    } else if (strcmp(key, "mac") == 0) {
        
    } else if (strcmp(key, "host_os") == 0) {
        drcom_config.host_os = value;
        printf("%s\n", drcom_config.host_os);
    } else if (strcmp(key, "KEEP_ALIVE_VERSION") == 0) {
        
    } else if (strcmp(key, "ror_version") == 0) {
        if (strcmp(value, "True")) {
            drcom_config.ror_version = 1;
        } else  {
            drcom_config.ror_version = 0;
        }
    } else {
        return 1;
    }

    return 0;
}

static int read_p_config(char *buf, int size) {
    if (verbose_flag) {
        printf("%s", buf);
    }

    char *delim = " ='\r\n";
    char *delim2 = "\\x";
    char *key;
    char *value;
    if (strlen(key = strtok(buf, delim))) {
        value = strtok(NULL, delim);
    }

    if (strcmp(key, "server") == 0) {
        strcpy(drcom_config.server, value);
        printf("%s\n", drcom_config.server);
    } else if (strcmp(key, "pppoe_flag") == 0) {
        value = strtok(value, delim2);
        sscanf(value, "%02x", &drcom_config.pppoe_flag);
        printf("0x%02x\n", drcom_config.pppoe_flag);
    } else if (strcmp(key, "keep_alive2_flag") == 0) {
        value = strtok(value, delim2);
        sscanf(value, "%02x", &drcom_config.keep_alive2_flag);
        printf("0x%02x\n", drcom_config.keep_alive2_flag);
    } else {
        return 1;
    }

    return 0;
}