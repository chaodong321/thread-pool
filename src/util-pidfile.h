#ifndef _UTIL_PIDFILE_H_
#define _UTIL_PIDFILE_H_

#ifdef __cplusplus
extern "C"{
#endif

#define PID_FILE_PATH "/var/run/tcp-proxy.pid"

int create_pidfile(const char *pid_file);
void remove_pidfile(const char *pid_file);
int is_pid_running(const char *pid_file);

#ifdef __cplusplus
}
#endif

#endif