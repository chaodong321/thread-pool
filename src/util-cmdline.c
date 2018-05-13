#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include "util-cmdline.h"

extern char *optarg;  
extern int optind, opterr, optopt;

static void usage(const char *program_name)
{
	printf("USAGE: %s [OPTIONS]\n\n", program_name);
	printf("\t-D                                   : run as daemon\n");
	printf("\t-c <path>                            : path to configuration file\n");
	printf("\t--logfile <path>                     : path to log file\n");
	printf("\t--pidfile <file>                     : write pid to this file\n");
	printf("\t--start                              : write pid to this file\n");
	printf("\t--stop                               : write pid to this file\n");
}

int parse_commandline(int argc, char **argv, prog_cmd *instance)
{
	if(instance == NULL){
		return -1;
	}
	
	char short_opts[] = "Dc:";

	struct option long_opts[] = {
		{"logfile",    required_argument, 0,  0 },
		{"pidfile",    required_argument, 0,  0 },
		{"start",      no_argument,       0,  0 },
		{"stop",       required_argument, 0,  0 },
		{0,            0,                 0,  0 }
	};

	int opt;

	int option_index = 0;
	while((opt = getopt_long(argc, argv, short_opts, long_opts, &option_index)) != -1){
		switch(opt){
		case 0:
			if (strcmp((long_opts[option_index]).name , "logfile") == 0){
				if(optarg != NULL) {
					memset(instance->logfile_path, 0, sizeof(instance->logfile_path));
                    strncpy(instance->logfile_path, optarg,
                            ((strlen(optarg) < sizeof(instance->logfile_path)) ?
                             (strlen(optarg) + 1) : sizeof(instance->logfile_path)));
					printf("%s: log file path:%s\n", __func__, instance->logfile_path);
				}
			}
			else if (strcmp((long_opts[option_index]).name , "pidfile") == 0){
				if(optarg != NULL) {
					memset(instance->logfile_path, 0, sizeof(instance->logfile_path));
                    strncpy(instance->logfile_path, optarg,
                            ((strlen(optarg) < sizeof(instance->logfile_path)) ?
                             (strlen(optarg) + 1) : sizeof(instance->logfile_path)));
					printf("%s: pid file path:%s\n", __func__, instance->logfile_path);
				}
			}
			else if (strcmp((long_opts[option_index]).name , "start") == 0){
				instance->start = 1;
			}
			else if (strcmp((long_opts[option_index]).name , "stop") == 0){
				instance->stop = 1;
			}
			else{
				usage(argv[0]);
				return -1;
			}
			break;
		case 'c':
			if(optarg != NULL) {
				memset(instance->config_path, 0, sizeof(instance->config_path));
                strncpy(instance->config_path, optarg,
                        ((strlen(optarg) < sizeof(instance->config_path)) ?
                         (strlen(optarg) + 1) : sizeof(instance->config_path)));
				printf("%s: config file path:%s\n", __func__, instance->config_path);
			}
			else{
				usage(argv[0]);
				return -1;
			}
			break;
		case 'D':
			instance->daemon = 1;
			printf("%s: program set to daemon\n", __func__);
			break;
		default:
			usage(argv[0]);
			return -1;
		}
	}
	
	return 0;
}

