#ifndef _TCP_SERVER_H
#define _TCP_SERVER_H

#ifdef __cplusplus
extern "C"{
#endif

int tcp_init();
void tcp_exit();
int tcp_accept();

#ifdef __cplusplus
}
#endif

#endif
