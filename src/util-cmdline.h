#ifndef _UTIL_CMDLINE_H_
#define _UTIL_CMDLINE_H_

#ifdef __cplusplus
extern "C"{
#endif

typedef struct _prog_cmd{
	int daemon;
	char config_path[128];
	char logfile_path[128];
	char pidfile_path[128];
	int start;
	int stop;
}prog_cmd;

int parse_commandline(int argc, char **argv, prog_cmd *instance);

#ifdef __cplusplus
}
#endif

#endif

