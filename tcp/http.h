#ifndef	___HTTP__
#define	___HTTP__

#include "arch.h"
#include "tcp.h"

err_t	http_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
err_t 	http_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
#endif

