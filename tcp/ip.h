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
 * $Id: ip.h,v 1.1.1.1 2011/06/10 08:06:31 yachang Exp $
 */
#ifndef __LWIP_IP_H__
#define __LWIP_IP_H__

#include "arch.h"

#include "def.h"
#include "pbuf.h"
//#include "lwip/ip_addr.h"
//#include "lwip/netif.h"

#include "err.h"

void ip_init(void);
//u8_t ip_lookup(void *header, struct netif *inp);
u8_t ip_lookup(void *header);
//struct netif *ip_route(struct ip_addr *dest);
//err_t ip_input(struct pbuf *p, struct netif *inp);
err_t ip_input(struct pbuf *p);
err_t ip_output(struct pbuf *p, u32_t *src, u32_t *dest,
		u8_t ttl, u8_t proto);
//err_t ip_output_if(struct pbuf *p, struct ip_addr *src, struct ip_addr *dest,
//		   u8_t ttl, u8_t proto,
//		   struct netif *netif);
err_t ip_output_if(struct pbuf *p, u32_t *src, u32_t *dest,
		   u8_t ttl, u8_t proto);

#define IP_HLEN 20

#define IP_PROTO_ICMP 1
#define IP_PROTO_UDP 17
#define IP_PROTO_UDPLITE 170
#define IP_PROTO_TCP 6

/* This is passed as the destination address to ip_output_if (not
   to ip_output), meaning that an IP header already is constructed
   in the pbuf. This is used when TCP retransmits. */
#ifdef IP_HDRINCL
#undef IP_HDRINCL
#endif /* IP_HDRINCL */
#define IP_HDRINCL  NULL

struct ip_hdr {
  /* version / header length / type of service */
  PACK_STRUCT_FIELD(u16_t _v_hl_tos);
  /* total length */
  PACK_STRUCT_FIELD(u16_t _len);
  /* identification */
  PACK_STRUCT_FIELD(u16_t _id);
  /* fragment offset field */
  PACK_STRUCT_FIELD(u16_t _offset);
#define IP_RF 0x8000        /* reserved fragment flag */
#define IP_DF 0x4000        /* dont fragment flag */
#define IP_MF 0x2000        /* more fragments flag */
#define IP_OFFMASK 0x1fff   /* mask for fragmenting bits */
  /* time to live / protocol*/
  PACK_STRUCT_FIELD(u16_t _ttl_proto);
  /* checksum */
  PACK_STRUCT_FIELD(u16_t _chksum);
  /* source and destination IP addresses */
//  PACK_STRUCT_FIELD(struct ip_addr src);
//  PACK_STRUCT_FIELD(struct ip_addr dest); 
  PACK_STRUCT_FIELD(u32_t src);
  PACK_STRUCT_FIELD(u32_t dest); 
} PACK_STRUCT_STRUCT;

#define IPH_V(hdr)  (NTOHS((hdr)->_v_hl_tos) >> 12)
#define IPH_HL(hdr) ((NTOHS((hdr)->_v_hl_tos) >> 8) & 0x0f)
#define IPH_TOS(hdr) HTONS((NTOHS((hdr)->_v_hl_tos) & 0xff))
#define IPH_LEN(hdr) ((hdr)->_len)
#define IPH_ID(hdr) ((hdr)->_id)
#define IPH_OFFSET(hdr) ((hdr)->_offset)
#define IPH_TTL(hdr) (NTOHS((hdr)->_ttl_proto) >> 8)
#define IPH_PROTO(hdr) (NTOHS((hdr)->_ttl_proto) & 0xff)
#define IPH_CHKSUM(hdr) ((hdr)->_chksum)

#define IPH_VHLTOS_SET(hdr, v, hl, tos) (hdr)->_v_hl_tos = HTONS(((v) << 12) | ((hl) << 8) | (tos))
#define IPH_LEN_SET(hdr, len) (hdr)->_len = (len)
#define IPH_ID_SET(hdr, id) (hdr)->_id = (id)
#define IPH_OFFSET_SET(hdr, off) (hdr)->_offset = (off)
#define IPH_TTL_SET(hdr, ttl) (hdr)->_ttl_proto = HTONS(IPH_PROTO(hdr) | ((ttl) << 8))
#define IPH_PROTO_SET(hdr, proto) (hdr)->_ttl_proto = HTONS((proto) | (IPH_TTL(hdr) << 8))
#define IPH_CHKSUM_SET(hdr, chksum) (hdr)->_chksum = (chksum)



#if IP_DEBUG
void ip_debug_print(struct pbuf *p);
#endif /* IP_DEBUG */

#endif /* __LWIP_IP_H__ */

//u16_t htons(u16_t n);
//u16_t ntohs(u16_t n);
//u32_t htonl(u32_t n);
//u32_t ntohl(u32_t n);

//u16_t inet_chksum_pseudo(struct pbuf *p,
//		   u32_t *src, u32_t *dest,
//		   u8_t proto, u32_t proto_len);



#ifndef __LWIP_IP_ADDR_H__
#define __LWIP_IP_ADDR_H__

#include "arch.h"

#define IP_ADDR_ANY 0

#define IP_ADDR_BROADCAST (&ip_addr_broadcast)

extern u32_t ip_addr_broadcast;

#define IP4_ADDR(ipaddr, a,b,c,d) (*((u32_t*)ipaddr)) = htonl(((u32_t)(a & 0xff) << 24) | ((u32_t)(b & 0xff) << 16) | \
                                                         ((u32_t)(c & 0xff) << 8) | (u32_t)(d & 0xff))

#define ip_addr_set(dest, src) (*((u32_t*)dest)) = \
                               (*((u32_t*)src) == IP_ADDR_ANY? IP_ADDR_ANY:\
				(*(u32_t*)src))
#define ip_addr_maskcmp(addr1, addr2, mask) ((*((u32_t*)addr1) & \
                                              (*((u32_t*)mask))) == \
                                             ((*((u32_t*)addr2)) & \
                                              (*((u32_t*)mask))))
#define ip_addr_cmp(addr1, addr2) ((*((u32_t*)addr1))== (*((u32_t*)addr2)))

#define ip_addr_isany(addr1) ((addr1) == NULL || ((*(u32_t*)addr1)) == 0)

#define ip_addr_isbroadcast(addr1, mask) ((((((*(u32_t*)addr1))) & ~(((*(u32_t*)mask)))) == \
					 (0xffffffff & ~(((*(u32_t*)mask))))) || \
                                         (((*(u32_t*)addr1) == 0xffffffff) || \
                                         (((*(u32_t*)addr1)) == 0x00000000))


#define ip_addr_ismulticast(addr1) ((((*(u32_t*)addr1)) & ntohl(0xf0000000)) == ntohl(0xe0000000))
				   

#define ip_addr_debug_print(ipaddr) DEBUGF(LWIP_DEBUG, ("%d.%d.%d.%d", \
		    (u8_t)(ntohl(((*(u32_t*)ipaddr))) >> 24) & 0xff, \
		    (u8_t)(ntohl(((*(u32_t*)ipaddr))) >> 16) & 0xff, \
		    (u8_t)(ntohl(((*(u32_t*)ipaddr))) >> 8) & 0xff, \
		    (u8_t)ntohl(((*(u32_t*)ipaddr))) & 0xff))


#define ip4_addr1(ipaddr) ((u8_t)(ntohl(((*(u32_t*)ipaddr))) >> 24) & 0xff)
#define ip4_addr2(ipaddr) ((u8_t)(ntohl(((*(u32_t*)ipaddr))) >> 16) & 0xff)
#define ip4_addr3(ipaddr) ((u8_t)(ntohl(((*(u32_t*)ipaddr))) >> 8) & 0xff)
#define ip4_addr4(ipaddr) ((u8_t)(ntohl(((*(u32_t*)ipaddr)))) & 0xff)
#endif /* __LWIP_IP_ADDR_H__ */
