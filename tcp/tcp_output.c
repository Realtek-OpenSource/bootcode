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
 * $Id: tcp_output.c,v 1.2 2012/11/14 07:48:54 tctsai Exp $
 */

/*-----------------------------------------------------------------------------------*/
/* tcp_output.c
 *
 * The output functions of TCP.
 *
 */
/*-----------------------------------------------------------------------------------*/

#include "debug.h"

#include "def.h"
#include "opt.h"

//#include "arch/lib.h"
#ifndef __ARCH_LIB_H__
#define __ARCH_LIB_H__

#ifndef _STRING_H_
#ifndef _STRING_H
int strlen(const char *str);
int strncmp(const char *str1, const char *str2, int len);
//void bcopy(const void *src, void *dest, int len);
//void xzero(void *data, int n);
#endif /* _STRING_H */
#endif /* _STRING_H_ */

#endif /* __ARCH_LIB_H__ */



#include "mem.h"
#include "memp.h"
//#include "lwip/sys.h"

//#include "lwip/netif.h"

//#include "lwip/inet.h"
#ifndef __LWIP_INET_H__
#define __LWIP_INET_H__

#include "arch.h"

#include "opt.h"
#include "pbuf.h"
//#include "lwip/ip_addr.h"

//u16_t inet_chksum(void *dataptr, u16_t len);
//u16_t inet_chksum_pbuf(struct pbuf *p);
//u16_t inet_chksum_pseudo(struct pbuf *p,
//			 struct ip_addr *src, struct ip_addr *dest,
//			 u8_t proto, u16_t proto_len);
u16_t inet_chksum_pseudo(struct pbuf *p,
		   u32_t *src, u32_t *dest,
		   u8_t proto, u32_t proto_len);

#ifdef HTONS
#undef HTONS
#endif /* HTONS */
#ifdef NTOHS
#undef NTOHS
#endif /* NTOHS */
#ifdef HTONL
#undef HTONL
#endif /* HTONL */
#ifdef NTOHL
#undef NTOHL
#endif /* NTOHL */

#ifndef HTONS
#   if BYTE_ORDER == BIG_ENDIAN
#      define HTONS(n) (n)
#      define htons(n) HTONS(n)
#   else /* BYTE_ORDER == BIG_ENDIAN */
#      define HTONS(n) (((((u16_t)(n) & 0xff)) << 8) | (((u16_t)(n) & 0xff00) >> 8))
#   endif /* BYTE_ORDER == BIG_ENDIAN */
#endif /* HTONS */

#ifdef NTOHS
#undef NTOHS
#endif /* NTOHS */

#ifdef ntohs
#undef ntohs
#endif /* ntohs */

#define NTOHS HTONS
#define ntohs htons


#ifndef HTONL
#   if BYTE_ORDER == BIG_ENDIAN
#      define HTONL(n) (n)
#      define htonl(n) HTONL(n)
#   else /* BYTE_ORDER == BIG_ENDIAN */
#      define HTONL(n) (((((u32_t)(n) & 0xff)) << 24) | \
                        ((((u32_t)(n) & 0xff00)) << 8) | \
                        ((((u32_t)(n) & 0xff0000)) >> 8) | \
                        ((((u32_t)(n) & 0xff000000)) >> 24))
#   endif /* BYTE_ORDER == BIG_ENDIAN */
#endif /* HTONL */

#ifdef ntohl
#undef ntohl
#endif /* ntohl */

#ifdef NTOHL
#undef NTOHL
#endif /* NTOHL */

#define NTOHL HTONL
#define ntohl htonl

#ifndef _MACHINE_ENDIAN_H_
#ifndef _NETINET_IN_H
#ifndef _LINUX_BYTEORDER_GENERIC_H

#if BYTE_ORDER == LITTLE_ENDIAN
u16_t htons(u16_t n);
u32_t htonl(u32_t n);
#else
#endif /* BYTE_ORDER == LITTLE_ENDIAN */

#endif /* _LINUX_BYTEORDER_GENERIC_H */
#endif /* _NETINET_IN_H */
#endif /* _MACHINE_ENDIAN_H_ */

#endif /* __LWIP_INET_H__ */


#include "tcp.h"

#include "stats.h"


#define MIN(x,y) (x) < (y)? (x): (y)



/* Forward declarations.*/
static void tcp_output_segment(struct tcp_seg *seg, struct tcp_pcb *pcb);

/*-----------------------------------------------------------------------------------*/
err_t
tcp_send_ctrl(struct tcp_pcb *pcb, u8_t flags)
{
  return tcp_enqueue(pcb, NULL, 0, flags, 1, NULL, 0);

}
/*-----------------------------------------------------------------------------------*/
err_t
tcp_write(struct tcp_pcb *pcb, const void *arg, u16_t len, u8_t copy)
{
  if(pcb->state == SYN_SENT ||
     pcb->state == SYN_RCVD ||
     pcb->state == ESTABLISHED ||
     pcb->state == CLOSE_WAIT) {
//	  printf("tcp_write here. length is %d.\n", len);
    if(len > 0) {
      return tcp_enqueue(pcb, (void *)arg, len, 0, copy, NULL, 0);
    }
    return ERR_OK;
  } else {
    return ERR_CONN;
  }
}
/*-----------------------------------------------------------------------------------*/
err_t
tcp_enqueue(struct tcp_pcb *pcb, void *arg, u16_t len,
	    u8_t flags, u8_t copy,
            u8_t *optdata, u8_t optlen)
{
  struct pbuf *p;
  struct tcp_seg *seg, *useg, *queue;
  u32_t left, seqno;
  u16_t seglen;
  void *ptr;
  u8_t queuelen;

  left = len;
  ptr = arg;
  
  if(len > pcb->snd_buf) {
    DEBUGF(TCP_OUTPUT_DEBUG, ("tcp_enqueue: too much data %d\n", len));
//	printf("error here 0\n");
    return ERR_MEM;
  }
  
  seqno = pcb->snd_lbb;
  
  queue = NULL;
  DEBUGF(TCP_QLEN_DEBUG, ("tcp_enqueue: %d\n", pcb->snd_queuelen));
  queuelen = pcb->snd_queuelen;
  if(queuelen >= TCP_SND_QUEUELEN) {
    DEBUGF(TCP_OUTPUT_DEBUG, ("tcp_enqueue: too long queue %d (max %d)\n", queuelen, TCP_SND_QUEUELEN));
    goto memerr;
  }   
  
#ifdef LWIP_DEBUG
  if(pcb->snd_queuelen != 0) {
    ASSERT("tcp_enqueue: valid queue length", pcb->unacked != NULL ||
	   pcb->unsent != NULL);      
  }
#endif /* LWIP_DEBUG */
  
  seg = NULL;
  seglen = 0;
  
  while(queue == NULL || left > 0) {
    
    seglen = left > pcb->mss? pcb->mss: left;
    
    /* allocate memory for tcp_seg, and fill in fields */
    seg = memp_malloc(MEMP_TCP_SEG);
    if(seg == NULL) {
      DEBUGF(TCP_OUTPUT_DEBUG, ("tcp_enqueue: could not allocate memory for tcp_seg\n"));
      goto memerr;
    }
    seg->next = NULL;
    seg->p = NULL;
    
    
    if(queue == NULL) {
      queue = seg;
    } else {
      for(useg = queue; useg->next != NULL; useg = useg->next);
      useg->next = seg;
    }
      
    /* If copy is set, memory should be allocated
       and data copied into pbuf, otherwise data comes from
       ROM or other static memory, and need not be copied. If
       optdata is != NULL, we have options instead of data. */
    if(optdata != NULL) {
      if((seg->p = pbuf_alloc(PBUF_TRANSPORT, optlen, PBUF_RAM)) == NULL) {
	goto memerr;
      }
      ++queuelen;
      seg->dataptr = seg->p->payload;
    } else if(copy) {
      if((seg->p = pbuf_alloc(PBUF_TRANSPORT, seglen, PBUF_RAM)) == NULL) {
	DEBUGF(TCP_OUTPUT_DEBUG, ("tcp_enqueue: could not allocate memory for pbuf copy\n"));	  
	goto memerr;
      }
      ++queuelen;
      if(arg != NULL) {
	bcopy(ptr, seg->p->payload, seglen);
      }
      seg->dataptr = seg->p->payload;
    } else {
      /* Do not copy the data. */
      if((p = pbuf_alloc(PBUF_TRANSPORT, seglen, PBUF_ROM)) == NULL) {
	DEBUGF(TCP_OUTPUT_DEBUG, ("tcp_enqueue: could not allocate memory for pbuf non-copy\n"));	  	  
	goto memerr;
      }
      ++queuelen;
      p->payload = ptr;
      seg->dataptr = ptr;
      if((seg->p = pbuf_alloc(PBUF_TRANSPORT, 0, PBUF_RAM)) == NULL) {
	pbuf_free(p);
	DEBUGF(TCP_OUTPUT_DEBUG, ("tcp_enqueue: could not allocate memory for header pbuf\n"));		  
	goto memerr;
      }
      ++queuelen;
      pbuf_chain(seg->p, p);
    }
    if(queuelen > TCP_SND_QUEUELEN) {
      DEBUGF(TCP_OUTPUT_DEBUG, ("tcp_enqueue: queue too long %d (%d)\n", queuelen, TCP_SND_QUEUELEN)); 	
      goto memerr;
    }
      
    seg->len = seglen;
    /*    if((flags & TCP_SYN) || (flags & TCP_FIN)) { 
      ++seg->len;
      }*/
      
    /* build TCP header */
    if(pbuf_header(seg->p, TCP_HLEN)) {
	
      DEBUGF(TCP_OUTPUT_DEBUG, ("tcp_enqueue: no room for TCP header in pbuf.\n"));
	
#ifdef TCP_STATS
      ++stats.tcp.err;
#endif /* TCP_STATS */
      goto memerr;
    }
    seg->tcphdr = seg->p->payload;
    seg->tcphdr->src = htons(pcb->local_port);
    seg->tcphdr->dest = htons(pcb->remote_port);
    seg->tcphdr->seqno = htonl(seqno);
    seg->tcphdr->urgp = 0;
    TCPH_FLAGS_SET(seg->tcphdr, flags);
//	printf("tcp src is %d, tcp dst is %d.\n", seg->tcphdr->src, seg->tcphdr->dest);
    /* don't fill in tcphdr->ackno and tcphdr->wnd until later */
      
    if(optdata == NULL) {
      TCPH_OFFSET_SET(seg->tcphdr, 5 << 4);
//	  printf("enqueue opt is zero.\n");
    } else {
      TCPH_OFFSET_SET(seg->tcphdr, (5 + optlen / 4) << 4);
//	  printf("enqueue opt is not zero.\n");
      /* Copy options into data portion of segment.
	 Options can thus only be sent in non data carrying
	 segments such as SYN|ACK. */
      bcopy(optdata, seg->dataptr, optlen);
    }
    DEBUGF(TCP_OUTPUT_DEBUG, ("tcp_enqueue: queueing %lu:%lu (0x%x)\n",
			      ntohl(seg->tcphdr->seqno),
			      ntohl(seg->tcphdr->seqno) + TCP_TCPLEN(seg),
			      flags));

    left -= seglen;
    seqno += seglen;
    ptr = (void *)((char *)ptr + seglen);
  }

    
  /* Go to the last segment on the ->unsent queue. */    
  if(pcb->unsent == NULL) {
    useg = NULL;
  } else {
    for(useg = pcb->unsent; useg->next != NULL; useg = useg->next);
  }
    
  /* If there is room in the last pbuf on the unsent queue,
     chain the first pbuf on the queue together with that. */
  if(useg != NULL &&
     TCP_TCPLEN(useg) != 0 &&
     !(TCPH_FLAGS(useg->tcphdr) & (TCP_SYN | TCP_FIN)) &&
     !(flags & (TCP_SYN | TCP_FIN)) &&
     useg->len + queue->len <= pcb->mss) {
    /* Remove TCP header from first segment. */
    pbuf_header(queue->p, -TCP_HLEN);
    pbuf_chain(useg->p, queue->p);
    useg->len += queue->len;
    useg->next = queue->next;
      
    DEBUGF(TCP_OUTPUT_DEBUG, ("tcp_output: chaining, new len %u\n", useg->len));
    if(seg == queue) {
      seg = NULL;
    }
    memp_free(MEMP_TCP_SEG, queue);
  } else {      
    if(useg == NULL) {
      pcb->unsent = queue;
    } else {
      useg->next = queue;
    }
  }
  if((flags & TCP_SYN) || (flags & TCP_FIN)) {
//	  printf("enqueue length add 1.\n");
    ++len;
  }
  pcb->snd_lbb += len;
  pcb->snd_buf -= len;
  pcb->snd_queuelen = queuelen;
  DEBUGF(TCP_QLEN_DEBUG, ("tcp_enqueue: %d (after enqueued)\n", pcb->snd_queuelen));
#ifdef LWIP_DEBUG
  if(pcb->snd_queuelen != 0) {
    ASSERT("tcp_enqueue: valid queue length", pcb->unacked != NULL ||
	   pcb->unsent != NULL);
      
  }
#endif /* LWIP_DEBUG */
    
  /* Set the PSH flag in the last segment that we enqueued, but only
     if the segment has data (indicated by seglen > 0). */
  if(seg != NULL && seglen > 0 && seg->tcphdr != NULL) {
    TCPH_FLAGS_SET(seg->tcphdr, TCPH_FLAGS(seg->tcphdr) | TCP_PSH);
  }
  
//  printf("enqueue OK.\n");
  return ERR_OK;
 memerr:
//  printf("enqueue ERROR.\n");
#ifdef TCP_STATS
  ++stats.tcp.memerr;
#endif /* TCP_STATS */

  if(queue != NULL) {
    tcp_segs_free(queue);
  }
#ifdef LWIP_DEBUG
    if(pcb->snd_queuelen != 0) {
      ASSERT("tcp_enqueue: valid queue length", pcb->unacked != NULL ||
	     pcb->unsent != NULL);
      
    }
#endif /* LWIP_DEBUG */
    DEBUGF(TCP_QLEN_DEBUG, ("tcp_enqueue: %d (with mem err)\n", pcb->snd_queuelen));
  return ERR_MEM;
}
/*-----------------------------------------------------------------------------------*/
/* find out what we can send and send it */
err_t
tcp_output(struct tcp_pcb *pcb)
{
  struct pbuf *p;
  struct tcp_hdr *tcphdr;
  struct tcp_seg *seg, *useg;
  u32_t wnd;
#if TCP_CWND_DEBUG
  int i = 0;
#endif /* TCP_CWND_DEBUG */
  
  wnd = MIN(pcb->snd_wnd, pcb->cwnd);

  
  seg = pcb->unsent;
//  if (seg ==NULL ) {
//	  printf("seg is null.\n");
//  } else {
//	  printf("seg len is %d.\n", seg->len);	  
//  }

  if(pcb->flags & TF_ACK_NOW) {
    /* If no segments are enqueued but we should send an ACK, we
       construct the ACK and send it. */
    pcb->flags &= ~(TF_ACK_DELAY | TF_ACK_NOW);
    p = pbuf_alloc(PBUF_TRANSPORT, 0, PBUF_RAM);
    if(p == NULL) {
      DEBUGF(TCP_OUTPUT_DEBUG, ("tcp_enqueue: (ACK) could not allocate pbuf\n"));
      return ERR_BUF;
    }
    DEBUGF(TCP_OUTPUT_DEBUG, ("tcp_enqueue: sending ACK for %lu\n", pcb->rcv_nxt));    
    if(pbuf_header(p, TCP_HLEN)) {
      DEBUGF(TCP_OUTPUT_DEBUG, ("tcp_enqueue: (ACK) no room for TCP header in pbuf.\n"));
      
#ifdef TCP_STATS
      ++stats.tcp.err;
#endif /* TCP_STATS */
      pbuf_free(p);
      return ERR_BUF;
    }
    
    tcphdr = p->payload;
    tcphdr->src = htons(pcb->local_port);
    tcphdr->dest = htons(pcb->remote_port);
    tcphdr->seqno = htonl(pcb->snd_nxt);
    tcphdr->ackno = htonl(pcb->rcv_nxt);
    TCPH_FLAGS_SET(tcphdr, TCP_ACK);
    tcphdr->wnd = htons(pcb->rcv_wnd);
    tcphdr->urgp = 0;
    TCPH_OFFSET_SET(tcphdr, 5 << 4);
    
    tcphdr->chksum = 0;
    tcphdr->chksum = inet_chksum_pseudo(p, &(pcb->local_ip), &(pcb->remote_ip),
					IP_PROTO_TCP, p->tot_len);

//	printf("Here ack.\n");
    ip_output(p, &(pcb->local_ip), &(pcb->remote_ip), TCP_TTL,
	      IP_PROTO_TCP);
    pbuf_free(p);

    return ERR_OK;
  } 
  
#if TCP_OUTPUT_DEBUG
  if(seg == NULL) {
    DEBUGF(TCP_OUTPUT_DEBUG, ("tcp_output: nothing to send\n"));
  }
#endif /* TCP_OUTPUT_DEBUG */
#if TCP_CWND_DEBUG
  if(seg == NULL) {
    DEBUGF(TCP_CWND_DEBUG, ("tcp_output: snd_wnd %lu, cwnd %lu, wnd %lu, seg == NULL, ack %lu\n",
                            pcb->snd_wnd, pcb->cwnd, wnd,
                            pcb->lastack));
  } else {
    DEBUGF(TCP_CWND_DEBUG, ("tcp_output: snd_wnd %lu, cwnd %lu, wnd %lu, effwnd %lu, seq %lu, ack %lu\n",
                            pcb->snd_wnd, pcb->cwnd, wnd,
                            ntohl(seg->tcphdr->seqno) - pcb->lastack + seg->len,
                            ntohl(seg->tcphdr->seqno), pcb->lastack));
  }
#endif /* TCP_CWND_DEBUG */
  
//  printf("seg is 0x%x, len is %d, wnd is %d.\n", seg, ntohl(seg->tcphdr->seqno) - pcb->lastack + seg->len, wnd);
  while(seg != NULL &&
	ntohl(seg->tcphdr->seqno) - pcb->lastack + seg->len <= wnd) {
    pcb->rtime = 0;
#if TCP_CWND_DEBUG
    DEBUGF(TCP_CWND_DEBUG, ("tcp_output: snd_wnd %lu, cwnd %lu, wnd %lu, effwnd %lu, seq %lu, ack %lu, i%d\n",
                            pcb->snd_wnd, pcb->cwnd, wnd,
                            ntohl(seg->tcphdr->seqno) + seg->len -
                            pcb->lastack,
                            ntohl(seg->tcphdr->seqno), pcb->lastack, i));
    ++i;
#endif /* TCP_CWND_DEBUG */

    pcb->unsent = seg->next;
    
    
    if(pcb->state != SYN_SENT) {
      TCPH_FLAGS_SET(seg->tcphdr, TCPH_FLAGS(seg->tcphdr) | TCP_ACK);
      pcb->flags &= ~(TF_ACK_DELAY | TF_ACK_NOW);
    }
    
    tcp_output_segment(seg, pcb);
    pcb->snd_nxt = ntohl(seg->tcphdr->seqno) + TCP_TCPLEN(seg);
    if(TCP_SEQ_LT(pcb->snd_max, pcb->snd_nxt)) {
      pcb->snd_max = pcb->snd_nxt;
    }
    /* put segment on unacknowledged list if length > 0 */
    if(TCP_TCPLEN(seg) > 0) {
      seg->next = NULL;
      if(pcb->unacked == NULL) {
        pcb->unacked = seg;
      } else {
        for(useg = pcb->unacked; useg->next != NULL; useg = useg->next);
        useg->next = seg;
      }
      /*      seg->rtime = 0;*/      
    } else {
      tcp_seg_free(seg);
    }
    seg = pcb->unsent;
  }  
  return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
static void
tcp_output_segment(struct tcp_seg *seg, struct tcp_pcb *pcb)
{
  u16_t len, tot_len;
//  struct netif *netif;

  /* The TCP header has already been constructed, but the ackno and
   wnd fields remain. */
  seg->tcphdr->ackno = htonl(pcb->rcv_nxt);

  /* silly window avoidance */
  if(pcb->rcv_wnd < pcb->mss) {
    seg->tcphdr->wnd = 0;
  } else {
    seg->tcphdr->wnd = htons(pcb->rcv_wnd);
  }

  /* If we don't have a local IP address, we get one by
     calling ip_route(). */
  if(ip_addr_isany(&(pcb->local_ip))) {
//    netif = ip_route(&(pcb->remote_ip));
//    if(netif == NULL) {
//      return;
//    }
//    ip_addr_set(&(pcb->local_ip), &(netif->ip_addr));
	  ip_addr_set(&(pcb->local_ip), &(netif.ip));
  }

  pcb->rtime = 0;
  
  if(pcb->rttest == 0) {
    pcb->rttest = tcp_ticks;
    pcb->rtseq = ntohl(seg->tcphdr->seqno);

    DEBUGF(TCP_RTO_DEBUG, ("tcp_output_segment: rtseq %lu\n", pcb->rtseq));
  }
  DEBUGF(TCP_OUTPUT_DEBUG, ("tcp_output_segment: %lu:%lu\n",
			    htonl(seg->tcphdr->seqno), htonl(seg->tcphdr->seqno) +
			    seg->len));

  seg->tcphdr->chksum = 0;
  seg->tcphdr->chksum = inet_chksum_pseudo(seg->p,
					   &(pcb->local_ip),
					   &(pcb->remote_ip),
					   IP_PROTO_TCP, seg->p->tot_len);
#ifdef TCP_STATS
  ++stats.tcp.xmit;
#endif /* TCP_STATS */

  len = seg->p->len;
  tot_len = seg->p->tot_len;

//  printf("seg len in tcp_output_segment is %d.\n", seg->len);
//  printf("local ip is %d.%d.%d.%d, ", (pcb->local_ip>>24)&0xff, (pcb->local_ip>>16)&0xff, (pcb->local_ip>>8)&0xff, (pcb->local_ip)&0xff);
//  printf("remote ip is %d.%d.%d.%d.\n", (pcb->remote_ip>>24)&0xff, (pcb->remote_ip>>16)&0xff, (pcb->remote_ip>>8)&0xff, (pcb->remote_ip)&0xff);
  ip_output(seg->p, &(pcb->local_ip), &(pcb->remote_ip), TCP_TTL,
	    IP_PROTO_TCP);
  seg->p->len = len;
  seg->p->tot_len = tot_len;
  seg->p->payload = seg->tcphdr;

}
/*-----------------------------------------------------------------------------------*/
void
tcp_rexmit_seg(struct tcp_pcb *pcb, struct tcp_seg *seg)
{
  u32_t wnd;
  u16_t len, tot_len;
  /*
  struct netif *netif;
  */
  
  DEBUGF(TCP_REXMIT_DEBUG, ("tcp_rexmit_seg: skickar %ld:%ld\n",
			    ntohl(seg->tcphdr->seqno),
			    ntohl(seg->tcphdr->seqno) + TCP_TCPLEN(seg)));

  wnd = MIN(pcb->snd_wnd, pcb->cwnd);
  
  if(ntohl(seg->tcphdr->seqno) - pcb->lastack + seg->len <= wnd) {

    /* Count the number of retranmissions. */
    ++pcb->nrtx;
    
//   if((netif = ip_route((struct ip_addr *)&(pcb->remote_ip))) == NULL) {
//      DEBUGF(TCP_OUTPUT_DEBUG, ("tcp_rexmit_segment: No route to 0x%lx\n", pcb->remote_ip.addr));
#ifdef TCP_STATS
//      ++stats.tcp.rterr;
#endif /* TCP_STATS */
//      return;
//    }

    seg->tcphdr->ackno = htonl(pcb->rcv_nxt);
    seg->tcphdr->wnd = htons(pcb->rcv_wnd);

    /* Recalculate checksum. */
    seg->tcphdr->chksum = 0;
    seg->tcphdr->chksum = inet_chksum_pseudo(seg->p,
                                             &(pcb->local_ip),
                                             &(pcb->remote_ip),
                                             IP_PROTO_TCP, seg->p->tot_len);
    
    len = seg->p->len;
    tot_len = seg->p->tot_len;
    pbuf_header(seg->p, IP_HLEN);
//    ip_output_if(seg->p, NULL, IP_HDRINCL, TCP_TTL, IP_PROTO_TCP, netif);
	ip_output_if(seg->p, NULL, IP_HDRINCL, TCP_TTL, IP_PROTO_TCP);
    seg->p->len = len;
    seg->p->tot_len = tot_len;
    seg->p->payload = seg->tcphdr;

#ifdef TCP_STATS
    ++stats.tcp.xmit;
    ++stats.tcp.rexmit;
#endif /* TCP_STATS */

    pcb->rtime = 0;
    
    /* Don't take any rtt measurements after retransmitting. */    
    pcb->rttest = 0;
  } else {
    DEBUGF(TCP_REXMIT_DEBUG, ("tcp_rexmit_seg: no room in window %lu to send %lu (ack %lu)\n",
                              wnd, ntohl(seg->tcphdr->seqno), pcb->lastack));
  }
}
/*-----------------------------------------------------------------------------------*/
void
//tcp_rst(u32_t seqno, u32_t ackno,
//	struct ip_addr *local_ip, struct ip_addr *remote_ip,
//	u16_t local_port, u16_t remote_port)
tcp_rst(u32_t seqno, u32_t ackno,
	u32_t *local_ip, u32_t *remote_ip,
	u16_t local_port, u16_t remote_port)
{
  struct pbuf *p;
  struct tcp_hdr *tcphdr;
  p = pbuf_alloc(PBUF_TRANSPORT, 0, PBUF_RAM);
  if(p == NULL) {
#if MEM_RECLAIM
    mem_reclaim(sizeof(struct pbuf));
    p = pbuf_alloc(PBUF_TRANSPORT, 0, PBUF_RAM);
#endif /* MEM_RECLAIM */    
    if(p == NULL) {
      DEBUGF(TCP_DEBUG, ("tcp_rst: could not allocate memory for pbuf\n"));
      return;
    }
  }
  if(pbuf_header(p, TCP_HLEN)) {
    DEBUGF(TCP_OUTPUT_DEBUG, ("tcp_send_data: no room for TCP header in pbuf.\n"));

#ifdef TCP_STATS
    ++stats.tcp.err;
#endif /* TCP_STATS */
    return;
  }

  tcphdr = p->payload;
  tcphdr->src = htons(local_port);
  tcphdr->dest = htons(remote_port);
  tcphdr->seqno = htonl(seqno);
  tcphdr->ackno = htonl(ackno);
  TCPH_FLAGS_SET(tcphdr, TCP_RST | TCP_ACK);
  tcphdr->wnd = 0;
  tcphdr->urgp = 0;
  TCPH_OFFSET_SET(tcphdr, 5 << 4);
  
  tcphdr->chksum = 0;
  tcphdr->chksum = inet_chksum_pseudo(p, local_ip, remote_ip,
				      IP_PROTO_TCP, p->tot_len);

#ifdef TCP_STATS
  ++stats.tcp.xmit;
#endif /* TCP_STATS */
  ip_output(p, local_ip, remote_ip, TCP_TTL, IP_PROTO_TCP);
  pbuf_free(p);
  DEBUGF(TCP_RST_DEBUG, ("tcp_rst: seqno %lu ackno %lu.\n", seqno, ackno));
}
/*-----------------------------------------------------------------------------------*/









