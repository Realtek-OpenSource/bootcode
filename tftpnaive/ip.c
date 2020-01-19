
/*
	Copyright 2001, 2002 Georges Menie (www.menie.org)

	This file is part of Tftpnaive.

    Tftpnaive is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Tftpnaive is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Tftpnaive; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "tftpnaive.h"
#include "net.h"

#include "../tcp/tcp.h"

void processIP (unsigned char *pkt, unsigned short len)
{
	register IPpkt *p = (IPpkt *) pkt;

	if (len >= sizeof (IPpkt)) {
		if ((p->vlt & 0xff00) == IPV4) {
			if ((p->frag & ~DONT_FRAGMENT_MASK) == 0) {
				if (netif.ip == 0 || p->dst_ip == netif.ip) {
					if (cks (&p->vlt, sizeof (IPpkt) - sizeof (EthHeader)) == 0) {
						if (p->prot == PROTO_ICMP) {
							processICMP (pkt, len);
						}
						else if (p->prot == PROTO_UDP) {
							processUDP (pkt, len);
						}
#ifdef CONFIG_RTL867X_LOADER_SUPPORT_HTTP_SERVER
					    else if (p->prot == PROTO_TCP) {
                        	unsigned char	*tp;
                        	struct pbuf *pb;
							struct pbuf *next;

							len = p->length;
//							printf("Get a IP packet. It's a http packet. Len is %d.\n", len);
							pb = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
//							pb = pbuf_alloc(PBUF_RAW, len, PBUF_ROM);	// ethhdr is not align
//							printf("pb is 0x%x, total len is %d.\n", pb, pb->tot_len);
							tp = (unsigned char*)&p->vlt;
							for(next=pb;len>0;next=next->next) {
								bcopy(tp, next->payload, next->len);
								tp += next->len;
								len -= next->len;
							}
//							pb->payload = (void*)&p->vlt;
//							printf("pb-payload is 0x%x\n", pb->payload);
							tcp_input(pb);
						}
#endif
					}
				}
			}
		}
	}
}

int sendIP (IPpkt * p, unsigned short len, unsigned char prot,
			unsigned int dst_ip)
{
	register unsigned char *dst;
	register int trycount;

	p->vlt = IPV4;
	p->length = len + sizeof (IPpkt) - sizeof (EthHeader);
	p->id = 0;
	p->frag = 0;
	p->ttl = 3;
	p->prot = prot;
	//p->src_ip = netif.ip;
	//p->dst_ip = dst_ip;
	_memcpy(&p->src_ip, &netif.ip, 4);
	_memcpy(&p->dst_ip, &dst_ip, 4);
	//
	p->cks = 0;
	p->cks = cks (&p->vlt, sizeof (IPpkt) - sizeof (EthHeader));

	if (dst_ip == -1) {
		_memset (p->ehdr.dst_hwadr, 0xff, 6);
	}
	else {
		for (trycount = 3; trycount; --trycount) {
			dst = lookupARP (p->dst_ip);
			if (dst) {
				_memcpy (p->ehdr.dst_hwadr, dst, 6);
				break;
			}
			sendARPRequest (netif.ip, p->dst_ip);
			busyWait (250, processPacket, 0);
		}
		dst = lookupARP (p->dst_ip);
		if (dst) {
			_memcpy (p->ehdr.dst_hwadr, dst, 6);
		}
		else {
			printf("lookupARP failed\n");
			return -1;
		}
	}

	_memcpy (p->ehdr.src_hwadr, netif.IEEEIA, 6);
	p->ehdr.frametype = FRAME_IP;

	return netif.send (p, len + sizeof (IPpkt));
}
