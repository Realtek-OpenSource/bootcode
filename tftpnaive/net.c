
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
#include "iob.h"

NetInterface netif;

//static BootData bdata;
extern BootData bdata;
BootData *tftp_req;

long Lan_Transmit(char*, unsigned long);

void _memcpy(dest, source, len)
    unsigned char* dest;
    const unsigned char* source;
    unsigned int  len;
{
    if (len == 0) return;
    do {
        *dest++ = *source++; /* ??? to be unrolled */
    } while (--len != 0);
}

void _memset(dest, val, len)
    unsigned char* dest;
    int val;
    unsigned int  len;
{
    if (len == 0) return;
    do {
        *dest++ = val;  /* ??? to be unrolled */
    } while (--len != 0);
}

#ifndef CONFIG_RTL865XC
/*xl_yue:20100208 remove cpu_tag or vlan_tag*/
static void remove_cpu_vlan_tag(unsigned char **pPkt, int *pLen, int tagLen)
{
       unsigned char hwaddr[12]; 
       int i;
       unsigned char *pBuf = *pPkt;

       for(i=0;i<12;i++)
       {
              hwaddr[i] = pBuf[i];
       }

       *pPkt += tagLen;
       *pLen -= tagLen;
       if(*pLen < 0){
              printf("packet length errror!\n");
       }
       pBuf = *pPkt;

       for(i=0;i<12;i++)
       {
              pBuf[i] = hwaddr[i];
       }
}
/*xl_yue:20100208 end*/
#endif

void processPacket (void)
{
#if 0
	register int idxo = idxoPBuf;
	register EthHeader *hdr;

	if (idxo != idxiPBuf) {
		idxo = (++idxo) % MAXPACKETBUF;
		hdr = (EthHeader *) packetBuf[idxo];
		if (hdr->frametype == FRAME_ARP) {
			processARP (packetBuf[idxo], packetBufLen[idxo]);
		}
		else if (hdr->frametype == FRAME_IP) {
			processIP (packetBuf[idxo], packetBufLen[idxo]);
		}
		idxoPBuf = idxo;
		++netif.RxPacket;
	}
#endif
    void * pPkt;
    unsigned long len;
    EthHeader * hdr; 
    
#ifdef CONFIG_RTL865XC    
    //if ( p_iob = Lan_Receive(pPkt,&len) )
    if (Lan_Receive(&pPkt,&len) )
    {
    	//pPkt = IOB_PKT_PTR(p_iob);
    	//len = IOB_PKT_LEN(p_iob);
        hdr = (EthHeader *) pPkt;
        if (hdr->frametype == FRAME_ARP)
            processARP (pPkt, len);
        else if (hdr->frametype == FRAME_IP)
            processIP (pPkt, len);
    }
#else
	IOB *p_iob;

    if ( p_iob = Lan_Receive() )
    {
    	pPkt = IOB_PKT_PTR(p_iob);
    	len = IOB_PKT_LEN(p_iob);
        hdr = (EthHeader *) pPkt;
		
       /*xl_yue:20100208 remove cpu_tag and vlan_tag at first*/
       if(hdr->frametype == 0x8899){
             //printf("cpu tag packet\n");
             remove_cpu_vlan_tag((unsigned char **)&pPkt,&len,4);
             hdr = (EthHeader *) pPkt;
       }
       if(hdr->frametype == 0x8100){
             //printf("vlan tag packet\n");
             remove_cpu_vlan_tag((unsigned char **)&pPkt,&len,4);
             hdr = (EthHeader *) pPkt;
       }
       /*xl_yue:20100208 end*/
	
        if (hdr->frametype == FRAME_ARP)
            processARP (pPkt, len);
        else if (hdr->frametype == FRAME_IP)
            processIP (pPkt, len);
    }

#endif
}

#if 0
void netStat (void)
{
	printf ("Total IRQ   : %d\n\r", netif.TotalIRQ);
	printf ("Total event : %d\n\r", netif.TotalEvent);
	printf ("Tx event    : %d\n\r", netif.TxEvent);
	printf ("Rx event    : %d\n\r", netif.RxEvent);
	printf ("Buf event   : %d\n\r", netif.BufEvent);
	printf ("Rx missed   : %d\n\r", netif.RxMiss);
	printf ("Rx dropped  : %d\n\r", netif.RxQFull);
	printf ("Collisions  : %d\n\r", netif.TxColl);
	printf ("Unknown ev  : %d\n\r", netif.UnknownEvent);
	printf ("Rx Packets  : %d\n\r", netif.RxPacket);
}
#endif

unsigned short cks_partial (void *buf, unsigned short len, long sum)
{
	while (len > 1) {
		//sum += *((unsigned short *) buf)++;
		sum += *(unsigned short *) buf;
		buf += sizeof(unsigned short);
		
		if (sum & 0x80000000)
			sum = (sum & 0xffff) + (sum >> 16);
		len -= 2;
	}
	if (len)
		sum += (unsigned short) ((*(unsigned char *) buf)<<8);
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return sum;
}

unsigned short cks (void *buf, unsigned short len)
{
	return ~cks_partial (buf, len, 0);
}

/* request an IP address then check for conflict
 */
static void getIP (int wait)
{
	netif.ip = 0;

	/* send the BOOTP request and wait for reply */
	if (sendBOOTPRequest () == 0)
		printf ("BOOTP request sent\n\r");
	busyWait (wait, processPacket, &netif.ip);

	/* check for IP address conflict */
	if (netCheckIP()) {
		/* this is to make sure we are sending BOOTP request
		 * with the delay specified.
		 */
		busyWait (wait, processPacket, 0);
	}
}

int netCheckIP (void)
{
	/* if we have our IP address, send
	 * a gratuituous ARP to check for IP conflict.
	 */
	if (netif.ip) {
		sendARPRequest (0, netif.ip);
		busyWait (250, processPacket, 0);
		if (netif.ip) {
			sendARPRequest (0, netif.ip);
			busyWait (250, processPacket, 0);
			if (netif.ip) {
				sendARPRequest (netif.ip, netif.ip);
				busyWait (250, processPacket, 0);
			}
		}
	}
	return (netif.ip == 0);
}

int netRequestIP (void)
{

	while (netif.ip == 0) {
		getIP (2000);
		if(check_break(0x1b))
			break;
	}

	return (netif.ip == 0);
}

int netTFTPGet (void)
{
	int lastbloc;

	if (netif.ip && tftp_req) {
		tftp_req->bcnt = tftp_req->sts = tftp_req->port = 0;
		lastbloc = tftp_req->bloc = 1;
		if (sendTFTPRequest () == 0) {
			printf ("TFTP request: ");
			printf ("server %d.%d.%d.%d, ",
					(tftp_req->server >> 24) & 0xff,
					(tftp_req->server >> 16) & 0xff,
					(tftp_req->server >> 8) & 0xff,
					tftp_req->server & 0xff);
			printf ("file '%s' ", tftp_req->file);
			printf ("buffer 0x%08x ", tftp_req->addr);
			printf ("\n\r");
			while (1) {
				//busyWait (3000, processPacket, &tftp_req->sts);
				{
					unsigned long start;
					start = get_sys_time();
					while(!timeout(start, 1000)) {
					    processPacket();
					    if (tftp_req->sts) goto check_sts;
					};
				}
check_sts:				
				if (tftp_req->sts > 0)
					return 0;
				if (tftp_req->sts < 0)
					return 1;
				if (lastbloc == tftp_req->bcnt)
					break;
				lastbloc = tftp_req->bcnt;
			}
			printf ("TFTP: receive timeout\n\r");
		}
		else
			printf ("TFTP: send error!\n\r");
	}
	return 1;
}

/* this is the entry point function
 * to initiate the network boot
 */
int bootpReceive (char * pMac, int * pSize, unsigned long ip, unsigned long server, char * file, unsigned long bufBase)
{
	int reqtry;
	
	_memcpy(netif.IEEEIA, pMac, 6);
	netif.send = Lan_Transmit;
	
	_memset (&bdata, 0, sizeof bdata);
	tftp_req = &bdata;

	if(ip != 0)
	{
		netif.ip = ip;
		tftp_req->server = server;
		strcpy(tftp_req->file, file);
	}
	else
	{
		/* request an IP address through BOOTP */
		netif.ip = 0;
		netRequestIP ();
		strcpy(file, tftp_req->file);
	}
	
	tftp_req->addr = (unsigned int) bufBase;
	tftp_req->strt = 0;

	/* if the IP is still valid (no conflict)
	 * and there is a valid TFTP request to be started
	 * try 5 times to download
	 */
	if (netif.ip && tftp_req) {
		tftp_req->strtf = tftp_req->inited = 1;
		for (reqtry = 5; reqtry && netif.ip; --reqtry) {
			if (netTFTPGet () == 0)
			{
			  *pSize = tftp_req->bcnt;
				return 0;
			}
		}
	}

	return 1;
}



#if TFTPNAIVE_TIMEREQUEST

int timestamp;

int netTimestampRequest(int ip)
{
	timestamp = 0;
	if (netif.ip && ip)
		if (sendICMPTimestampRequest (ip) == 0)
			busyWait (500, processPacket, &timestamp);
	return timestamp;
}

#endif
