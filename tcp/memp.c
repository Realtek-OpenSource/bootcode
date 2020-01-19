/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: memp.c,v 1.1.1.1 2011/06/10 08:06:31 yachang Exp $
 */

#include "lwipopts.h"

#include "memp.h"

#include "pbuf.h"
//#include "lwip/udp.h"
#include "tcp.h"
//#include "lwip/api.h"
//#include "lwip/api_msg.h"
//#include "lwip/tcpip.h"

//#include "lwip/sys.h"
#include "stats.h"


#include "../inc/board.h"


//	also in http.c
struct memp {
  struct memp *next;
};

struct mem {
  mem_size_t next, prev;
  u8_t used;
#if MEM_ALIGNMENT == 2
  u8_t dummy;
  u16_t dummy2;
#endif /* MEM_ALIGNEMNT == 2 */
};

/*
*********************************
just for sizeof
*********************************
*/

#if 0
typedef	unsigned int sys_mbox_t;
typedef	unsigned int sys_sem_t;

struct udp_pcb {
  struct udp_pcb *next;

  u32_t local_ip, remote_ip;
  u16_t local_port, remote_port;
  
  u8_t flags;
  u16_t chksum_len;
  
  void (* recv)(void *arg, struct udp_pcb *pcb, struct pbuf *p,
		u32_t *addr, u16_t port);
  void *recv_arg;  
};

struct netbuf {
  struct pbuf *p, *ptr;
  void *fromaddr;
  u16_t fromport;
  err_t err;
};

enum netconn_type {
  NETCONN_TCP,
  NETCONN_UDP,
  NETCONN_UDPLITE,
  NETCONN_UDPNOCHKSUM
};

enum netconn_state {
  NETCONN_NONE,
  NETCONN_WRITE,
  NETCONN_ACCEPT,
  NETCONN_RECV,
  NETCONN_CONNECT,
  NETCONN_CLOSE
};
struct netconn {
  enum netconn_type type;
  enum netconn_state state;
  union {
    struct tcp_pcb *tcp;
    struct udp_pcb *udp;
  } pcb;
  err_t err;
  sys_mbox_t mbox;
  sys_mbox_t recvmbox;
  sys_mbox_t acceptmbox;
  sys_sem_t sem;
};

enum api_msg_type {
  API_MSG_NEWCONN,
  API_MSG_DELCONN,
  
  API_MSG_BIND,
  API_MSG_CONNECT,

  API_MSG_LISTEN,
  API_MSG_ACCEPT,

  API_MSG_SEND,
  API_MSG_RECV,
  API_MSG_WRITE,

  API_MSG_CLOSE,
  
  API_MSG_MAX
};

struct api_msg_msg {
  struct netconn *conn;
  enum netconn_type conntype;
  union {
    struct pbuf *p;   
    struct  {
      void *ipaddr;
      u16_t port;
    } bc;
    struct {
      void *dataptr;
      u16_t len;
      unsigned char copy;
    } w;    
    sys_mbox_t mbox;
    u16_t len;
  } msg;
};

struct api_msg {
  enum api_msg_type type;
  struct api_msg_msg msg;
};

enum tcpip_msg_type {
  TCPIP_MSG_API,
  TCPIP_MSG_INPUT
};

struct tcpip_msg {
  enum tcpip_msg_type type;
  void *sem;
  union {
    struct api_msg *apimsg;
    struct {
      struct pbuf *p;
      struct netif *netif;
    } inp;
  } msg;
};

struct sys_timeout {
  struct sys_timeout *next;
  u16_t time;
  void* h;
  void *arg;
};



#endif




















//static struct memp *memp_tab[MEMP_MAX];

/*
static const u16_t memp_sizes[MEMP_MAX] = {
  sizeof(struct pbuf),
  sizeof(struct udp_pcb),
  sizeof(struct tcp_pcb),
  sizeof(struct tcp_pcb_listen),
  sizeof(struct tcp_seg),
  sizeof(struct netbuf),
  sizeof(struct netconn),
  sizeof(struct api_msg),
  sizeof(struct tcpip_msg),
  sizeof(struct sys_timeout)
};

static const u16_t memp_num[MEMP_MAX] = {
  MEMP_NUM_PBUF,
  MEMP_NUM_UDP_PCB,
  MEMP_NUM_TCP_PCB,
  MEMP_NUM_TCP_PCB_LISTEN,
  MEMP_NUM_TCP_SEG,
  MEMP_NUM_NETBUF,
  MEMP_NUM_NETCONN,
  MEMP_NUM_API_MSG,
  MEMP_NUM_TCPIP_MSG,
  MEMP_NUM_SYS_TIMEOUT
};

static u8_t memp_memory[(MEMP_NUM_PBUF *
			 MEM_ALIGN_SIZE(sizeof(struct pbuf) +
					sizeof(struct memp)) +
			MEMP_NUM_UDP_PCB *
			 MEM_ALIGN_SIZE(sizeof(struct udp_pcb) +
					sizeof(struct memp)) +
			MEMP_NUM_TCP_PCB *
			 MEM_ALIGN_SIZE(sizeof(struct tcp_pcb) +
					sizeof(struct memp)) +
			MEMP_NUM_TCP_PCB_LISTEN *
			 MEM_ALIGN_SIZE(sizeof(struct tcp_pcb_listen) +
					sizeof(struct memp)) +
			MEMP_NUM_TCP_SEG *
			 MEM_ALIGN_SIZE(sizeof(struct tcp_seg) +
					sizeof(struct memp)) +
			MEMP_NUM_NETBUF *
			 MEM_ALIGN_SIZE(sizeof(struct netbuf) +
					sizeof(struct memp)) +
			MEMP_NUM_NETCONN *
			 MEM_ALIGN_SIZE(sizeof(struct netconn) +
					sizeof(struct memp)) +
			MEMP_NUM_API_MSG *
			 MEM_ALIGN_SIZE(sizeof(struct api_msg) +
					sizeof(struct memp)) +
			MEMP_NUM_TCPIP_MSG *
			 MEM_ALIGN_SIZE(sizeof(struct tcpip_msg) +
					sizeof(struct memp)) +
			MEMP_NUM_SYS_TIMEOUT *
			 MEM_ALIGN_SIZE(sizeof(struct sys_timeout) +
					sizeof(struct memp)))];

*/

//(MEM_ALIGN_SIZE((PBUF_POOL_BUFSIZE+sizeof(struct pbuf))*PBUF_POOL_SIZE))
static struct memp  **memp_tab=(struct memp**)(((unsigned char*)MEM_ALIGN(DRAM_MAP_DLOAD_BUF_ADDR))+MEM_ALIGN_SIZE((PBUF_POOL_BUFSIZE+sizeof(struct pbuf))*PBUF_POOL_SIZE)+
											   MEM_ALIGN_SIZE(MEM_SIZE + sizeof(struct mem)));
//	size if sizeof(struct memp*)*MEMP_MAX

static u16_t *const memp_sizes = (u16_t*)(((unsigned char*)MEM_ALIGN(DRAM_MAP_DLOAD_BUF_ADDR))+
										  MEM_ALIGN_SIZE(sizeof(struct memp*)*MEMP_MAX)+
										  (MEM_ALIGN_SIZE((PBUF_POOL_BUFSIZE+sizeof(struct pbuf))*PBUF_POOL_SIZE))+
										  MEM_ALIGN_SIZE(MEM_SIZE + sizeof(struct mem)));

//static const u16_t memp_sizes[MEMP_MAX] = {
//  sizeof(struct pbuf),
////  sizeof(struct udp_pcb),
//  sizeof(struct tcp_pcb),
//  sizeof(struct tcp_pcb_listen),
//  sizeof(struct tcp_seg),
////  sizeof(struct netbuf),
////  sizeof(struct netconn),
////  sizeof(struct api_msg),
////  sizeof(struct tcpip_msg),
////  sizeof(struct sys_timeout)
//};



static u16_t *const memp_num = (u16_t*)(((unsigned char*)MEM_ALIGN(DRAM_MAP_DLOAD_BUF_ADDR))+
										MEM_ALIGN_SIZE((sizeof(struct memp*)+sizeof(u16_t))*MEMP_MAX)+
										(MEM_ALIGN_SIZE((PBUF_POOL_BUFSIZE+sizeof(struct pbuf))*PBUF_POOL_SIZE))+
										MEM_ALIGN_SIZE(MEM_SIZE + sizeof(struct mem)));

//static const u16_t memp_num[MEMP_MAX] = {
//  MEMP_NUM_PBUF,
////  MEMP_NUM_UDP_PCB,
//  MEMP_NUM_TCP_PCB,
//  MEMP_NUM_TCP_PCB_LISTEN,
//  MEMP_NUM_TCP_SEG,
////  MEMP_NUM_NETBUF,
////  MEMP_NUM_NETCONN,
////  MEMP_NUM_API_MSG,
////  MEMP_NUM_TCPIP_MSG,
////  MEMP_NUM_SYS_TIMEOUT
//};


static u8_t *const memp_memory = (u8_t*)(((unsigned char*)MEM_ALIGN(DRAM_MAP_DLOAD_BUF_ADDR))+
										 MEM_ALIGN_SIZE((sizeof(struct memp*)+sizeof(u16_t)*2)*MEMP_MAX)+
										 (MEM_ALIGN_SIZE((PBUF_POOL_BUFSIZE+sizeof(struct pbuf))*PBUF_POOL_SIZE))+
										 MEM_ALIGN_SIZE(MEM_SIZE + sizeof(struct mem)));

//static u8_t memp_memory[(MEMP_NUM_PBUF *
//			 MEM_ALIGN_SIZE(sizeof(struct pbuf) +
//					sizeof(struct memp)) +
////			MEMP_NUM_UDP_PCB *
////			 MEM_ALIGN_SIZE(sizeof(struct udp_pcb) +
////					sizeof(struct memp)) +
//			MEMP_NUM_TCP_PCB *
//			 MEM_ALIGN_SIZE(sizeof(struct tcp_pcb) +
//					sizeof(struct memp)) +
//			MEMP_NUM_TCP_PCB_LISTEN *
//			 MEM_ALIGN_SIZE(sizeof(struct tcp_pcb_listen) +
//					sizeof(struct memp)) +
//			MEMP_NUM_TCP_SEG *
//			 MEM_ALIGN_SIZE(sizeof(struct tcp_seg) +
////					sizeof(struct memp)) +
////			MEMP_NUM_NETBUF *
////			 MEM_ALIGN_SIZE(sizeof(struct netbuf) +
////					sizeof(struct memp)) +
////			MEMP_NUM_NETCONN *
////			 MEM_ALIGN_SIZE(sizeof(struct netconn) +
////					sizeof(struct memp)) +
////			MEMP_NUM_API_MSG *
////			 MEM_ALIGN_SIZE(sizeof(struct api_msg) +
////					sizeof(struct memp)) +
////			MEMP_NUM_TCPIP_MSG *
////			 MEM_ALIGN_SIZE(sizeof(struct tcpip_msg) +
////					sizeof(struct memp)) +
////			MEMP_NUM_SYS_TIMEOUT *
////			 MEM_ALIGN_SIZE(sizeof(struct sys_timeout) +
//					sizeof(struct memp)))];



#if MEMP_RECLAIM
struct memp_reclaim_ {
  struct memp_reclaim_ *next;
  memp_reclaim_func f;
  void *arg;  
};

static struct memp_reclaim_ *memp_reclaim_funcs[MEMP_MAX];

static u8_t memp_reclaim(memp_t type);
#endif /* MEMP_RECLAIM */

/*-----------------------------------------------------------------------------------*/
#if DEFINE_SYS_SEM
static sys_sem_t mutex;
#endif
/*-----------------------------------------------------------------------------------*/
#ifdef LWIP_DEBUG
static int
memp_sanity(void)
{
  int i, c;
  struct memp *m, *n;
  
  for(i = 0; i < MEMP_MAX; i++) {
    for(m = memp_tab[i]; m != NULL; m = m->next) {
      c = 1;
      for(n = memp_tab[i]; n != NULL; n = n->next) {
	if(n == m) {
	  --c;
	}
	if(c < 0)
	  abort();
      }
    }
  }
  return 1;
}
#endif /* LWIP_DEBUG */
/*-----------------------------------------------------------------------------------*/
void
memp_init(void)
{
  struct memp *m, *memp;
  u16_t i, j;
  u16_t size;

//  printf("memp_tab is 0x%x, MEMP_MAX is %d.\n", memp_tab, MEMP_MAX);
//  printf("memp_sizes is 0x%x, gap is %d.\n", memp_sizes, (int)memp_sizes-(int)memp_tab);
//  printf("memp_num is 0x%x, gap is %d.\n", memp_num, (int)memp_num-(int)memp_sizes);
//  printf("memp_memory is 0x%x, gap is %d.\n", memp_memory, (int)memp_memory-(int)memp_num);

  memp_sizes[MEMP_PBUF] = sizeof(struct pbuf);
  memp_sizes[MEMP_TCP_PCB] = sizeof(struct tcp_pcb);
  memp_sizes[MEMP_TCP_PCB_LISTEN] = sizeof(struct tcp_pcb_listen);
  memp_sizes[MEMP_TCP_SEG] = sizeof(struct tcp_seg);

  memp_num[MEMP_PBUF] = MEMP_NUM_PBUF;
  memp_num[MEMP_TCP_PCB] = MEMP_NUM_TCP_PCB;
  memp_num[MEMP_TCP_PCB_LISTEN] = MEMP_NUM_TCP_PCB_LISTEN;
  memp_num[MEMP_TCP_SEG] = MEMP_NUM_TCP_SEG;

#ifdef MEMP_STATS
  for(i = 0; i < MEMP_MAX; ++i) {
    stats.memp[i].used = stats.memp[i].max =
      stats.memp[i].err = stats.memp[i].reclaimed = 0;
    stats.memp[i].avail = memp_num[i];
  }
#endif /* MEMP_STATS */

//  memp = (struct memp *)&memp_memory[0];
  memp = (struct memp *)memp_memory;
  for(i = 0; i < MEMP_MAX; ++i) {
    size = MEM_ALIGN_SIZE(memp_sizes[i] + sizeof(struct memp));
    if(memp_num[i] > 0) {
      memp_tab[i] = memp;
      m = memp;
      
      for(j = 0; j < memp_num[i]; ++j) {
	m->next = (struct memp *)MEM_ALIGN((u8_t *)m + size);
	memp = m;
	m = m->next;
      }
      memp->next = NULL;
      memp = m;
    } else {
      memp_tab[i] = NULL;
    }
  }

#if DEFINE_SYS_SEM
  mutex = sys_sem_new(1);
#endif

#if MEMP_RECLAIM
  for(i = 0; i < MEMP_MAX; ++i) {
    memp_reclaim_funcs[i] = NULL;
  }
#endif /* MEMP_RECLAIM */
  
}
/*-----------------------------------------------------------------------------------*/
void *
memp_malloc(memp_t type)
{
  struct memp *memp;

  ASSERT("memp_malloc: type < MEMP_MAX", type < MEMP_MAX);

  memp = memp_tab[type];
  
  if(memp != NULL) {    
    memp_tab[type] = memp->next;    
    memp->next = NULL;
#ifdef MEMP_STATS
    ++stats.memp[type].used;
    if(stats.memp[type].used > stats.memp[type].max) {
      stats.memp[type].max = stats.memp[type].used;
    }
#endif /* MEMP_STATS */
    ASSERT("memp_malloc: memp properly aligned",
	   ((u32_t)MEM_ALIGN((u8_t *)memp + sizeof(struct memp)) % MEM_ALIGNMENT) == 0);

    return MEM_ALIGN((u8_t *)memp + sizeof(struct memp));
  } else {
    DEBUGF(MEMP_DEBUG, ("memp_malloc: out of memory in pool %d\n", type));
#ifdef MEMP_STATS
    ++stats.memp[type].err;
#endif /* MEMP_STATS */
    return NULL;
  }
}
/*-----------------------------------------------------------------------------------*/
void *
memp_mallocp(memp_t type)
{
  void *mem;
#if DEFINE_SYS_SEM
  sys_sem_wait(mutex);
#endif
  mem = memp_malloc(type);
#if DEFINE_SYS_SEM
  sys_sem_signal(mutex);
#endif
  return mem;
}
/*-----------------------------------------------------------------------------------*/
void *
memp_malloc2(memp_t type)
{
  void *mem;

  mem = memp_malloc(type);
#if MEMP_RECLAIM
  if(mem == NULL) {
    memp_reclaim(type);
    mem = memp_malloc(type);
  }
#endif /* MEMP_RECLAIM */
  return mem;
}
/*-----------------------------------------------------------------------------------*/
void *
memp_realloc(memp_t fromtype, memp_t totype, void *mem)
{
  void *rmem;
  u16_t size;
  
  if(mem == NULL) {
    return NULL;
  }
  
  rmem = memp_malloc(totype);
  if(rmem != NULL) { 
    size = memp_sizes[totype];
    if(memp_sizes[fromtype] < size) {
      size = memp_sizes[fromtype];
    }
    bcopy(mem, rmem, size);
    memp_free(fromtype, mem);
  }
  return rmem;
}
/*-----------------------------------------------------------------------------------*/
void
memp_free(memp_t type, void *mem)
{
  struct memp *memp;

  if(mem == NULL) {
    return;
  }
  memp = (struct memp *)((u8_t *)mem - sizeof(struct memp));

#ifdef MEMP_STATS
  stats.memp[type].used--; 
#endif /* MEMP_STATS */
  
  memp->next = memp_tab[type]; 
  memp_tab[type] = memp;

  ASSERT("memp sanity", memp_sanity());
  return;
}
/*-----------------------------------------------------------------------------------*/
void 
memp_freep(memp_t type, void *mem)
{
#if DEFINE_SYS_SEM
  sys_sem_wait(mutex);
#endif
  memp_free(type, mem);
#if DEFINE_SYS_SEM
  sys_sem_signal(mutex);
#endif
}
/*-----------------------------------------------------------------------------------*/
#if MEMP_RECLAIM
static u8_t
memp_reclaim(memp_t type)
{
  struct memp_reclaim_ *mr;
    
  for(mr = memp_reclaim_funcs[type]; mr != NULL; mr = mr->next) {
    DEBUGF(MEMP_DEBUG, ("memp_reclaim: calling reclaimer for type %d\n", type));
    if(mr->f(mr->arg, type) != 0) {
#ifdef MEMP_STATS
      ++stats.memp[type].reclaimed;
#endif /* MEMP_STATS */
      return 1;
    }
  }
  return 0;
}
/*-----------------------------------------------------------------------------------*/
void
memp_register_reclaim(memp_t type, memp_reclaim_func f, void *arg)
{
  struct memp_reclaim_ *mr;

  mr = mem_malloc(sizeof(struct memp_reclaim_));
  if(mr == NULL) {
    return;
  }
  mr->next = memp_reclaim_funcs[type];
  memp_reclaim_funcs[type] = mr;  
  mr->f = f;
  mr->arg = arg;
  DEBUGF(MEMP_DEBUG, ("memp_register_reclaim: registering reclaimer for type %d\n", type));
}
#endif /* MEMP_RECLAIM */
/*-----------------------------------------------------------------------------------*/
