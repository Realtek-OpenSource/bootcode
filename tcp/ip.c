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
 * $Id: ip.c,v 1.3 2012/11/14 07:48:54 tctsai Exp $
 */


/*-----------------------------------------------------------------------------------*/
/* ip.c
 *
 * This is the code for the IP layer.
 *
 */   
/*-----------------------------------------------------------------------------------*/

#include "debug.h"

#include "def.h"
#include "mem.h"
#include "ip.h"
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



//#include "lwip/netif.h"
//#include "lwip/icmp.h"
//#include "lwip/udp.h"
#include "tcp.h"

#include "stats.h"
#include "../tftpnaive/net.h"


//#include "arch/perf.h"
#ifndef __PERF_H__
#define __PERF_H__

#define PERF_START    /* null definition */
#define PERF_STOP(x)  /* null definition */

#endif /* __PERF_H__ */


#if LWIP_DHCP
//#include "lwip/dhcp.h"
#endif /* LWIP_DHCP */

/*-----------------------------------------------------------------------------------*/
/* ip_init:
 *
 * Initializes the IP layer.
 */
/*-----------------------------------------------------------------------------------*/
void
ip_init(void)
{
}
/*-----------------------------------------------------------------------------------*/
/* ip_lookup:
 *
 * An experimental feature that will be changed in future versions. Do
 * not depend on it yet...
 */
/*-----------------------------------------------------------------------------------*/
#if 0
#ifdef LWIP_DEBUG
u8_t
ip_lookup(void *header, struct netif *inp)
{
  struct ip_hdr *iphdr;

  iphdr = header;

  /* Refuse anything that isn't IPv4. */
  if(IPH_V(iphdr) != 4) {
    return 0;
  }

  /* Immediately accept/decline packets that are fragments or has
     options. */
#if IP_REASSEMBLY == 0
  /*  if((IPH_OFFSET(iphdr) & htons(IP_OFFMASK | IP_MF)) != 0) {
    return 0;
    }*/
#endif /* IP_REASSEMBLY == 0 */

#if IP_OPTIONS == 0
  if(IPH_HL(iphdr) != 5) {
    return 0;
  }
#endif /* IP_OPTIONS == 0 */
  
  switch(IPH_PROTO(iphdr)) {
#if LWIP_UDP > 0
  case IP_PROTO_UDP:
    return udp_lookup(iphdr, inp);
    break;
#endif /* LWIP_UDP */
#if LWIP_TCP > 0    
  case IP_PROTO_TCP:
    return 1;
#endif /* LWIP_TCP */
  case IP_PROTO_ICMP:
    return 1;
    break;
  default:
    return 0;
  }
}
#endif /* LWIP_DEBUG */
/*-----------------------------------------------------------------------------------*/
/* ip_route:
 *
 * Finds the appropriate network interface for a given IP address. It
 * searches the list of network interfaces linearly. A match is found
 * if the masked IP address of the network interface equals the masked
 * IP address given to the function.
 */
/*-----------------------------------------------------------------------------------*/
struct netif *
ip_route(struct ip_addr *dest)
{
  struct netif *netif;
  
  for(netif = netif_list; netif != NULL; netif = netif->next) {
    if(ip_addr_maskcmp(dest, &(netif->ip_addr), &(netif->netmask))) {
      return netif;
    }
  }

  return netif_default;
}
#if IP_FORWARD
/*-----------------------------------------------------------------------------------*/
/* ip_forward:
 *
 * Forwards an IP packet. It finds an appropriate route for the
 * packet, decrements the TTL value of the packet, adjusts the
 * checksum and outputs the packet on the appropriate interface.
 */
/*-----------------------------------------------------------------------------------*/
static void
ip_forward(struct pbuf *p, struct ip_hdr *iphdr, struct netif *inp)
{
  static struct netif *netif;
  
  PERF_START;
  
  if((netif = ip_route((struct ip_addr *)&(iphdr->dest))) == NULL) {

    DEBUGF(IP_DEBUG, ("ip_forward: no forwarding route for 0x%lx found\n",
		      iphdr->dest.addr));

    return;
  }

  /* Don't forward packets onto the same network interface on which
     they arrived. */
  if(netif == inp) {
    DEBUGF(IP_DEBUG, ("ip_forward: not forward packets back on incoming interface.\n"));

    return;
  }
  
  /* Decrement TTL and send ICMP if ttl == 0. */
  IPH_TTL_SET(iphdr, IPH_TTL(iphdr) - 1);
  if(IPH_TTL(iphdr) == 0) {
    /* Don't send ICMP messages in response to ICMP messages */
    if(IPH_PROTO(iphdr) != IP_PROTO_ICMP) {
      icmp_time_exceeded(p, ICMP_TE_TTL);
    }
    return;       
  }
  
  /* Incremental update of the IP checksum. */
  if(IPH_CHKSUM(iphdr) >= htons(0xffff - 0x100)) {
    IPH_CHKSUM_SET(iphdr, IPH_CHKSUM(iphdr) + htons(0x100) + 1);
  } else {
    IPH_CHKSUM_SET(iphdr, IPH_CHKSUM(iphdr) + htons(0x100));
  }

  DEBUGF(IP_DEBUG, ("ip_forward: forwarding packet to 0x%lx\n",
		    iphdr->dest.addr));

#ifdef IP_STATS
  ++stats.ip.fw;
  ++stats.ip.xmit;
#endif /* IP_STATS */

  PERF_STOP("ip_forward");
  
  netif->output(netif, p, (struct ip_addr *)&(iphdr->dest));
}
#endif /* IP_FORWARD */
/*-----------------------------------------------------------------------------------*/
/* ip_reass:
 *
 * Tries to reassemble a fragmented IP packet.
 */
/*-----------------------------------------------------------------------------------*/
#define IP_REASSEMBLY 0
#define IP_REASS_BUFSIZE 5760
#define IP_REASS_MAXAGE 10

#if IP_REASSEMBLY
static u8_t ip_reassbuf[IP_HLEN + IP_REASS_BUFSIZE];
static u8_t ip_reassbitmap[IP_REASS_BUFSIZE / (8 * 8)];
static const u8_t bitmap_bits[8] = {0xff, 0x7f, 0x3f, 0x1f,
				    0x0f, 0x07, 0x03, 0x01};
static u16_t ip_reasslen;
static u8_t ip_reassflags;
#define IP_REASS_FLAG_LASTFRAG 0x01
static u8_t ip_reasstmr;

static struct pbuf *
ip_reass(struct pbuf *p)
{
  struct pbuf *q;
  struct ip_hdr *fraghdr, *iphdr;
  u16_t offset, len;
  u16_t i;
  
  iphdr = (struct ip_hdr *)ip_reassbuf;
  fraghdr = (struct ip_hdr *)p->payload;

  /* If ip_reasstmr is zero, no packet is present in the buffer, so we
     write the IP header of the fragment into the reassembly
     buffer. The timer is updated with the maximum age. */
  if(ip_reasstmr == 0) {
    DEBUGF(IP_REASS_DEBUG, ("ip_reass: new packet\n"));
    bcopy(fraghdr, iphdr, IP_HLEN);
    ip_reasstmr = IP_REASS_MAXAGE;
    ip_reassflags = 0;
    /* Clear the bitmap. */
    xzero(ip_reassbitmap, sizeof(ip_reassbitmap));
  }

  /* Check if the incoming fragment matches the one currently present
     in the reasembly buffer. If so, we proceed with copying the
     fragment into the buffer. */
  if(ip_addr_cmp(&iphdr->src, &fraghdr->src) &&
     ip_addr_cmp(&iphdr->dest, &fraghdr->dest) &&
     IPH_ID(iphdr) == IPH_ID(fraghdr)) {
    DEBUGF(IP_REASS_DEBUG, ("ip_reass: matching old packet\n"));
    /* Find out the offset in the reassembly buffer where we should
       copy the fragment. */
    len = ntohs(IPH_LEN(fraghdr)) - IPH_HL(fraghdr) * 4;
    offset = (ntohs(IPH_OFFSET(fraghdr)) & IP_OFFMASK) * 8;

    /* If the offset or the offset + fragment length overflows the
       reassembly buffer, we discard the entire packet. */
    if(offset > IP_REASS_BUFSIZE ||
       offset + len > IP_REASS_BUFSIZE) {
      DEBUGF(IP_REASS_DEBUG, ("ip_reass: fragment outside of buffer (%d:%d/%d).\n",
			      offset, offset + len, IP_REASS_BUFSIZE));
      ip_reasstmr = 0;
      goto nullreturn;
    }

    /* Copy the fragment into the reassembly buffer, at the right
       offset. */
    DEBUGF(IP_REASS_DEBUG, ("ip_reass: copying with offset %d into %d:%d\n",
			    offset, IP_HLEN + offset, IP_HLEN + offset + len));
    bcopy((u8_t *)fraghdr + IPH_HL(fraghdr) * 4,
	  &ip_reassbuf[IP_HLEN + offset], len);

    /* Update the bitmap. */
    if(offset / (8 * 8) == (offset + len) / (8 * 8)) {
      DEBUGF(IP_REASS_DEBUG, ("ip_reass: updating single byte in bitmap.\n"));
      /* If the two endpoints are in the same byte, we only update
	 that byte. */
      ip_reassbitmap[offset / (8 * 8)] |=
	bitmap_bits[(offset / 8 ) & 7] &
	~bitmap_bits[((offset + len) / 8 ) & 7];
    } else {
      /* If the two endpoints are in different bytes, we update the
	 bytes in the endpoints and fill the stuff inbetween with
	 0xff. */
      ip_reassbitmap[offset / (8 * 8)] |= bitmap_bits[(offset / 8 ) & 7];
      DEBUGF(IP_REASS_DEBUG, ("ip_reass: updating many bytes in bitmap (%d:%d).\n",
			      1 + offset / (8 * 8), (offset + len) / (8 * 8)));
      for(i = 1 + offset / (8 * 8); i < (offset + len) / (8 * 8); ++i) {
	ip_reassbitmap[i] = 0xff;
      }      
      ip_reassbitmap[(offset + len) / (8 * 8)] |= ~bitmap_bits[((offset + len) / 8 ) & 7];
    }
    
    /* If this fragment has the More Fragments flag set to zero, we
       know that this is the last fragment, so we can calculate the
       size of the entire packet. We also set the
       IP_REASS_FLAG_LASTFRAG flag to indicate that we have received
       the final fragment. */

    if((ntohs(IPH_OFFSET(fraghdr)) & IP_MF) == 0) {
      ip_reassflags |= IP_REASS_FLAG_LASTFRAG;
      ip_reasslen = offset + len;
      DEBUGF(IP_REASS_DEBUG, ("ip_reass: last fragment seen, total len %d\n", ip_reasslen));
    }
    
    /* Finally, we check if we have a full packet in the buffer. We do
       this by checking if we have the last fragment and if all bits
       in the bitmap are set. */
    if(ip_reassflags & IP_REASS_FLAG_LASTFRAG) {
      /* Check all bytes up to and including all but the last byte in
	 the bitmap. */
      for(i = 0; i < ip_reasslen / (8 * 8) - 1; ++i) {
	if(ip_reassbitmap[i] != 0xff) {
	  DEBUGF(IP_REASS_DEBUG, ("ip_reass: last fragment seen, bitmap %d/%d failed (%x)\n", i, ip_reasslen / (8 * 8) - 1, ip_reassbitmap[i]));
	  goto nullreturn;
	}
      }
      /* Check the last byte in the bitmap. It should contain just the
	 right amount of bits. */
      if(ip_reassbitmap[ip_reasslen / (8 * 8)] !=
	 (u8_t)~bitmap_bits[ip_reasslen / 8 & 7]) {
	DEBUGF(IP_REASS_DEBUG, ("ip_reass: last fragment seen, bitmap %d didn't contain %x (%x)\n",
				ip_reasslen / (8 * 8), ~bitmap_bits[ip_reasslen / 8 & 7],
				ip_reassbitmap[ip_reasslen / (8 * 8)]));
	goto nullreturn;
      }

      /* Pretend to be a "normal" (i.e., not fragmented) IP packet
	 from now on. */
      IPH_OFFSET_SET(iphdr, 0);
      IPH_CHKSUM_SET(iphdr, 0);
      IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, IP_HLEN));
      
      /* If we have come this far, we have a full packet in the
	 buffer, so we allocate a pbuf and copy the packet into it. We
	 also reset the timer. */
      ip_reasstmr = 0;
      pbuf_free(p);
      p = pbuf_alloc(PBUF_LINK, ip_reasslen, PBUF_POOL);
      if(p != NULL) {
	i = 0;
	for(q = p; q != NULL; q = q->next) {
	  /* Copy enough bytes to fill this pbuf in the chain. The
	     avaliable data in the pbuf is given by the q->len
	     variable. */
	  DEBUGF(IP_REASS_DEBUG, ("ip_reass: bcopy from %p (%d) to %p, %d bytes\n",
				  &ip_reassbuf[i], i, q->payload, q->len > ip_reasslen - i? ip_reasslen - i: q->len));
	  bcopy(&ip_reassbuf[i], q->payload,
		q->len > ip_reasslen - i? ip_reasslen - i: q->len);
	  i += q->len;
	}
      }
      DEBUGF(IP_REASS_DEBUG, ("ip_reass: p %p\n", p));
      return p;
    }
  }

 nullreturn:
  pbuf_free(p);
  return NULL;
}
#endif /* IP_REASSEMBLY */
#endif


/*-----------------------------------------------------------------------------------*/
/* ip_input:
 *
 * This function is called by the network interface device driver when
 * an IP packet is received. The function does the basic checks of the
 * IP header such as packet size being at least larger than the header
 * size etc. If the packet was not destined for us, the packet is
 * forwarded (using ip_forward). The IP checksum is always checked.
 *
 * Finally, the packet is sent to the upper layer protocol input function.
 */
/*-----------------------------------------------------------------------------------*/
// does not use this functions as the input function
#if 0
err_t
ip_input(struct pbuf *p, struct netif *inp) {
  static struct ip_hdr *iphdr;
  static struct netif *netif;
  static u8_t hl;

  
  
#ifdef IP_STATS
  ++stats.ip.recv;
#endif /* IP_STATS */

  /* identify the IP header */
  iphdr = p->payload;
  if(IPH_V(iphdr) != 4) {
    DEBUGF(IP_DEBUG, ("IP packet dropped due to bad version number %d\n", IPH_V(iphdr)));
#if IP_DEBUG
    ip_debug_print(p);
#endif /* IP_DEBUG */
    pbuf_free(p);
#ifdef IP_STATS
    ++stats.ip.err;
    ++stats.ip.drop;
#endif /* IP_STATS */
    return ERR_OK;
  }
  
  hl = IPH_HL(iphdr);
  
  if(hl * 4 > p->len) {
    DEBUGF(IP_DEBUG, ("IP packet dropped due to too short packet %d\n", p->len));

    pbuf_free(p);
#ifdef IP_STATS
    ++stats.ip.lenerr;
    ++stats.ip.drop;
#endif /* IP_STATS */
    return ERR_OK;
  }

  /* verify checksum */
  if(inet_chksum(iphdr, hl * 4) != 0) {

    DEBUGF(IP_DEBUG, ("IP packet dropped due to failing checksum 0x%x\n", inet_chksum(iphdr, hl * 4)));
#if IP_DEBUG
    ip_debug_print(p);
#endif /* IP_DEBUG */
    pbuf_free(p);
#ifdef IP_STATS
    ++stats.ip.chkerr;
    ++stats.ip.drop;
#endif /* IP_STATS */
    return ERR_OK;
  }
  
  /* Trim pbuf. This should have been done at the netif layer,
     but we'll do it anyway just to be sure that its done. */
  pbuf_realloc(p, ntohs(IPH_LEN(iphdr)));

  /* is this packet for us? */
  for(netif = netif_list; netif != NULL; netif = netif->next) {

    DEBUGF(IP_DEBUG, ("ip_input: iphdr->dest 0x%lx netif->ip_addr 0x%lx (0x%lx, 0x%lx, 0x%lx)\n",
		      iphdr->dest.addr, netif->ip_addr.addr,
		      iphdr->dest.addr & netif->netmask.addr,
		      netif->ip_addr.addr & netif->netmask.addr,
		      iphdr->dest.addr & ~(netif->netmask.addr)));

    if(ip_addr_isany(&(netif->ip_addr)) ||
       ip_addr_cmp(&(iphdr->dest), &(netif->ip_addr)) ||
       (ip_addr_isbroadcast(&(iphdr->dest), &(netif->netmask)) &&
	ip_addr_maskcmp(&(iphdr->dest), &(netif->ip_addr), &(netif->netmask))) ||
       ip_addr_cmp(&(iphdr->dest), IP_ADDR_BROADCAST)) {
      break;
    }
  }

#if LWIP_DHCP
  /* If a DHCP packet has arrived on the interface, we pass it up the
     stack regardless of destination IP address. The reason is that
     DHCP replies are sent to the IP adress that will be given to this
     node (as recommended by RFC 1542 section 3.1.1, referred by RFC
     2131). */
  if(IPH_PROTO(iphdr) == IP_PROTO_UDP &&
     ((struct udp_hdr *)((u8_t *)iphdr + IPH_HL(iphdr) * 4/sizeof(u8_t)))->src ==
     DHCP_SERVER_PORT) {
    netif = inp;
  }  
#endif /* LWIP_DHCP */
	  
  if(netif == NULL) {
    /* packet not for us, route or discard */
    DEBUGF(IP_DEBUG, ("ip_input: packet not for us.\n"));
#if IP_FORWARD
    if(!ip_addr_isbroadcast(&(iphdr->dest), &(inp->netmask))) {
      ip_forward(p, iphdr, inp);
    }
#endif /* IP_FORWARD */
    pbuf_free(p);
    return ERR_OK;
  }

#if IP_REASSEMBLY
  if((IPH_OFFSET(iphdr) & htons(IP_OFFMASK | IP_MF)) != 0) {
    p = ip_reass(p);
    if(p == NULL) {
      return ERR_OK;
    }
    iphdr = p->payload;
  }
#else /* IP_REASSEMBLY */
  if((IPH_OFFSET(iphdr) & htons(IP_OFFMASK | IP_MF)) != 0) {
    pbuf_free(p);
    DEBUGF(IP_DEBUG, ("IP packet dropped since it was fragmented (0x%x).\n",
		      ntohs(IPH_OFFSET(iphdr))));
#ifdef IP_STATS
    ++stats.ip.opterr;
    ++stats.ip.drop;
#endif /* IP_STATS */
    return ERR_OK;
  }
#endif /* IP_REASSEMBLY */
  
#if IP_OPTIONS == 0
  if(hl * 4 > IP_HLEN) {
    DEBUGF(IP_DEBUG, ("IP packet dropped since there were IP options.\n"));

    pbuf_free(p);    
#ifdef IP_STATS
    ++stats.ip.opterr;
    ++stats.ip.drop;
#endif /* IP_STATS */
    return ERR_OK;
  }  
#endif /* IP_OPTIONS == 0 */


  /* send to upper layers */
#if IP_DEBUG
  DEBUGF(IP_DEBUG, ("ip_input: \n"));
  ip_debug_print(p);
  DEBUGF(IP_DEBUG, ("ip_input: p->len %d p->tot_len %d\n", p->len, p->tot_len));
#endif /* IP_DEBUG */   

  switch(IPH_PROTO(iphdr)) {
#if LWIP_UDP > 0    
  case IP_PROTO_UDP:
    udp_input(p, inp);
    break;
#endif /* LWIP_UDP */
#if LWIP_TCP > 0    
  case IP_PROTO_TCP:
    tcp_input(p, inp);
    break;
#endif /* LWIP_TCP */
  case IP_PROTO_ICMP:
    icmp_input(p, inp);
    break;
  default:
    /* send ICMP destination protocol unreachable unless is was a broadcast */
    if(!ip_addr_isbroadcast(&(iphdr->dest), &(inp->netmask)) &&
       !ip_addr_ismulticast(&(iphdr->dest))) {
      p->payload = iphdr;
      icmp_dest_unreach(p, ICMP_DUR_PROTO);
    }
    pbuf_free(p);

    DEBUGF(IP_DEBUG, ("Unsupported transportation protocol %d\n", IPH_PROTO(iphdr)));

#ifdef IP_STATS
    ++stats.ip.proterr;
    ++stats.ip.drop;
#endif /* IP_STATS */

  }
  return ERR_OK;
}
#endif


/*-----------------------------------------------------------------------------------*/
/* ip_output_if:
 *
 * Sends an IP packet on a network interface. This function constructs
 * the IP header and calculates the IP header checksum. If the source
 * IP address is NULL, the IP address of the outgoing network
 * interface is filled in as source address.
 */
/*-----------------------------------------------------------------------------------*/

#include "../inc/board.h"

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


struct IPData {
	EthHeader		ehdr;
	unsigned char	data[TCP_MSS+4];
};
//static	struct IPData	sendPkt;
static	struct IPData	* const sendPkt = (struct IPData *)(((unsigned char*)MEM_ALIGN(DRAM_MAP_DLOAD_BUF_ADDR))+
															MEM_ALIGN_SIZE((sizeof(struct memp*)+
															sizeof(u16_t)*2)
															*MEMP_MAX)+(MEMP_NUM_PBUF *
															MEM_ALIGN_SIZE(sizeof(struct pbuf) +
															sizeof(struct memp)) +
															MEMP_NUM_TCP_PCB *
															MEM_ALIGN_SIZE(sizeof(struct tcp_pcb) +
															sizeof(struct memp)) +
															MEMP_NUM_TCP_PCB_LISTEN *
															MEM_ALIGN_SIZE(sizeof(struct tcp_pcb_listen) +
															sizeof(struct memp)) +
															MEMP_NUM_TCP_SEG *
															MEM_ALIGN_SIZE(sizeof(struct tcp_seg) +
															sizeof(struct memp)))+
															(PBUF_POOL_SIZE*
															MEM_ALIGN_SIZE(PBUF_POOL_BUFSIZE + 
															sizeof(struct pbuf))) +
															MEM_ALIGN_SIZE(MEM_SIZE + 
															sizeof(struct mem))
															);

err_t
ip_output_if(struct pbuf *p, u32_t *src, u32_t *dest,
		   u8_t ttl, u8_t proto)
{
	u16_t	len;
	struct pbuf *next;
	IPpkt	*ip;
	TCPpkt	*tcp;

	if(pbuf_header(p, IP_HLEN)) {
		DEBUGF(IP_DEBUG, ("ip_output: not enough room for IP header in pbuf\n"));
#ifdef IP_STATS
		++stats.ip.err;
#endif /* IP_STATS */
		pbuf_free(p);
		return ERR_BUF;
	}
	len = 0;
	for (next=p;next!=NULL;next=next->next) {
//		printf("pbuf len is %d.\n", next->len);
//		bcopy(((unsigned char*)next->payload), &sendPkt.data[len], next->len);
		bcopy(((unsigned char*)next->payload), &sendPkt->data[len], next->len);
		len += next->len;
	}

//	ip = (IPpkt*)&sendPkt;
//	tcp = (TCPpkt*)&sendPkt;
	ip = (IPpkt*)sendPkt;
	tcp = (TCPpkt*)sendPkt;
//	printf("ip version is %d, ip head len is %d, ip len is %d, src ip is %d.%d.%d.%d, dst ip is %d.%d.%d.%d.\n",
//		(ip->vlt>>12)&0xf, ((ip->vlt>>8)&0xf)<<2, ip->length,
//		(ip->src_ip>>24)&0xff, (ip->src_ip>>16)&0xff, (ip->src_ip>>8)&0xff, (ip->src_ip)&0xff, 
//		(ip->dst_ip>>24)&0xff, (ip->dst_ip>>16)&0xff, (ip->dst_ip>>8)&0xff, (ip->dst_ip)&0xff);
//	printf("tcp src port is %d, tcp dst port is %d, tcp head len is %d.\n", tcp->src_port, tcp->dst_port,
//		((tcp->_offset_flags>>12)&0xf)<<2);
//	printf("len is %d, p total len is %d.\n", len, p->tot_len);
//	if(len!=p->tot_len) {
//		printf("Error in %d, of file %s\n", __LINE__, __FILE__);
//	} else {
//		printf("Send here.\n");
//		sendIP((IPpkt*)&sendPkt, (unsigned short)len-IP_HLEN, proto, *dest);
		sendIP((IPpkt*)sendPkt, (unsigned short)len-IP_HLEN, proto, *dest);
//	}
	return ERR_OK;
}

/*-----------------------------------------------------------------------------------*/
/* ip_output:
 *
 * Simple interface to ip_output_if. It finds the outgoing network
 * interface and calls upon ip_output_if to do the actual work.
 */
/*-----------------------------------------------------------------------------------*/
err_t
ip_output(struct pbuf *p, u32_t *src_ip, u32_t *dest_ip,
		u8_t ttl, u8_t proto)
{
  return ip_output_if(p, src_ip, dest_ip, ttl, proto);
}
/*-----------------------------------------------------------------------------------*/
#if IP_DEBUG
void
ip_debug_print(struct pbuf *p)
{
  struct ip_hdr *iphdr = p->payload;
  u8_t *payload;

  payload = (u8_t *)iphdr + IP_HLEN/sizeof(u8_t);
  
  DEBUGF(IP_DEBUG, ("IP header:\n"));
  DEBUGF(IP_DEBUG, ("+-------------------------------+\n"));
  DEBUGF(IP_DEBUG, ("|%2d |%2d |   %2d  |      %4d     | (v, hl, tos, len)\n",
		    IPH_V(iphdr),
		    IPH_HL(iphdr),
		    IPH_TOS(iphdr),
		    ntohs(IPH_LEN(iphdr))));
  DEBUGF(IP_DEBUG, ("+-------------------------------+\n"));
  DEBUGF(IP_DEBUG, ("|    %5d      |%d%d%d|    %4d   | (id, flags, offset)\n",
		    ntohs(IPH_ID(iphdr)),
		    ntohs(IPH_OFFSET(iphdr)) >> 15 & 1,
		    ntohs(IPH_OFFSET(iphdr)) >> 14 & 1,
		    ntohs(IPH_OFFSET(iphdr)) >> 13 & 1,
		    ntohs(IPH_OFFSET(iphdr)) & IP_OFFMASK));
  DEBUGF(IP_DEBUG, ("+-------------------------------+\n"));
  DEBUGF(IP_DEBUG, ("|   %2d  |   %2d  |    0x%04x     | (ttl, proto, chksum)\n",
		    IPH_TTL(iphdr),
		    IPH_PROTO(iphdr),
		    ntohs(IPH_CHKSUM(iphdr))));
  DEBUGF(IP_DEBUG, ("+-------------------------------+\n"));
  DEBUGF(IP_DEBUG, ("|  %3ld  |  %3ld  |  %3ld  |  %3ld  | (src)\n",
		    ntohl(iphdr->src.addr) >> 24 & 0xff,
		    ntohl(iphdr->src.addr) >> 16 & 0xff,
		    ntohl(iphdr->src.addr) >> 8 & 0xff,
		    ntohl(iphdr->src.addr) & 0xff));
  DEBUGF(IP_DEBUG, ("+-------------------------------+\n"));
  DEBUGF(IP_DEBUG, ("|  %3ld  |  %3ld  |  %3ld  |  %3ld  | (dest)\n",
		    ntohl(iphdr->dest.addr) >> 24 & 0xff,
		    ntohl(iphdr->dest.addr) >> 16 & 0xff,
		    ntohl(iphdr->dest.addr) >> 8 & 0xff,
		    ntohl(iphdr->dest.addr) & 0xff));
  DEBUGF(IP_DEBUG, ("+-------------------------------+\n"));
}
#endif /* IP_DEBUG */
/*-----------------------------------------------------------------------------------*/




#if BYTE_ORDER==LITTLE_ENDIAN
u16_t
htons(u16_t n)
{
  return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}
/*-----------------------------------------------------------------------------------*/
u16_t
ntohs(u16_t n)
{
  return htons(n);
}
/*-----------------------------------------------------------------------------------*/
u32_t
htonl(u32_t n)
{
  return ((n & 0xff) << 24) |
    ((n & 0xff00) << 8) |
    ((n & 0xff0000) >> 8) |
    ((n & 0xff000000) >> 24);
}
/*-----------------------------------------------------------------------------------*/
u32_t
ntohl(u32_t n)
{
  return htonl(n);
}
/*-----------------------------------------------------------------------------------*/
#endif


/*
void
bcopy(const void *src, void *dst, unsigned int size)
{
  char *csrc, *cdst;
  unsigned int i;

  csrc = (char *)src;
  cdst = dst;
  
  for(i = 0; i < size; ++i) {
    cdst[i] = csrc[i];
  }    
}
*/

/*-----------------------------------------------------------------------------------*/
//void
//xzero(void *s, int n)
//{
//  for(--n ;n >= 0; --n) {
//    ((char *)s)[n] = 0;
//  }
//}
/*-----------------------------------------------------------------------------------*/



  
/*-----------------------------------------------------------------------------------*/
 /* chksum:
 *
 * Sums up all 16 bit words in a memory portion. Also includes any odd byte.
 * This function is used by the other checksum functions.
 *
 * For now, this is not optimized. Must be optimized for the particular processor
 * arcitecture on which it is to run. Preferebly coded in assembler.*/
 /*-----------------------------------------------------------------------------------*/
static unsigned int chksum(void *dataptr, u16_t len)
{
	unsigned int acc;

	for(acc = 0; len > 1; len -= 2) {
		//acc += *((u16_t *)dataptr)++;
		acc += *(u16_t *)dataptr;
		dataptr += sizeof(u16_t);
	}

	/* add up any odd byte */
	if(len == 1) {
		acc += ((u16_t)((*(u8_t *)dataptr) & 0xff) << 8);
	}

	return acc;
}

/*-----------------------------------------------------------------------------------*/
/* inet_chksum_pseudo:
 *
 * Calculates the pseudo Internet checksum used by TCP and UDP for a pbuf chain.
 */
/*-----------------------------------------------------------------------------------*/
u16_t
inet_chksum_pseudo(struct pbuf *p,
		   u32_t *src, u32_t *dest,
		   u8_t proto, u32_t proto_len)
{
  u32_t acc;
  struct pbuf *q;
  u8_t swapped;

  acc = 0;
  swapped = 0;
  for(q = p; q != NULL; q = q->next) {    
    acc += chksum(q->payload, q->len);
    while(acc >> 16) {
      acc = (acc & 0xffff) + (acc >> 16);
    }
    if(q->len % 2 != 0) {
      swapped = 1 - swapped;
      acc = ((acc & 0xff) << 8) | ((acc & 0xff00) >> 8);
    }
  }

  if(swapped) {
    acc = ((acc & 0xff) << 8) | ((acc & 0xff00) >> 8);
  }
  
  acc += (*src&0xffff);
  acc += ((*src>>16)&0xffff);
  acc += (*dest&0xffff);
  acc += ((*dest>>16)&0xffff);

  acc += (u32_t)htons((u16_t)proto);
  acc += (u32_t)htons(proto_len); 

  while(acc >> 16) {
    acc = (acc & 0xffff) + (acc >> 16);
  }
  return ~(acc & 0xffff);
}

