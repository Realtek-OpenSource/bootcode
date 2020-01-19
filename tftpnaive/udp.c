
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
#ifdef SUPPORT_MULT_UPGRADE
#include "board.h"
#endif

typedef struct {
	int sa;
	int da;
	short pt;
	short ln;
} fakehdr;


#ifdef SUPPORT_MULT_UPGRADE
extern board_param_t bParam;
int multicast_frame_start=0;
int multicast_frame_alive=0;
int multicast_frame_stop=0;
unsigned int total_crc32=0;
unsigned int total_len=0;
int multicast_frame_finish=0;
int multicast_recvd_first_pkt_num = -1;
#define MUTLICAST_FRAME_LENGTH   1024
#define SEQNUM_NOT_EXIST              0x0
#define IMAGE_NOT_INTEGRITY	    0x12
#define IMAGE_INTEGRITY			    0x13	

#define		MUP_DATATYPE_NOP	0x0
#define     MUP_DATATYPE_NORM	0x1
#define		MUP_DATATYPE_LAST	0x2
#define     MUP_DATATYPE_INFO   0x4
#define     MUP_DATATYPE_MASK   0x7

typedef struct {
	UDPpkt udphdr;
#if 1
	unsigned int hl;
#else
	unsigned short datatype;	
	unsigned short datalen;
#endif
	unsigned 	char   dataseqnum[4];
	unsigned char   datacrc32[4];
	unsigned char   imagelength[4];
	unsigned char   imagecrc32[4];
	unsigned char package_id[22];
	unsigned char product_id[20];	
	unsigned char image_data[1024];	
#define MUP_DATA_ALIVE_ON 1
#define MUP_DATA_ALIVE_OFF 2
}__attribute__ ((__packed__)) MUTICAST_FRAME,*MUTICAST_FRAME_Pt;


typedef struct partition_hdr_t
{
	char			name[20];
	unsigned int		addrstart;
	unsigned int		addrend;
	unsigned char	cover_flag;
	unsigned char	nop[3];
}__attribute__ ((__packed__)) partition_hdr;

typedef struct partition_info_t
{
	int				valid;	/*1: valid, 0: invalid*/
	int				type;	/*boot/image/config*/
#define PARTI_TYPE_BOOT 0x1
#define PARTI_TYPE_IMG	0x2
#define PARTI_TYPE_CONF	0x4
	unsigned int		addrstart;
	unsigned int		addrend;
	unsigned char	cover_flag;
}partition_info;

#ifndef	NULL
#define	NULL	0
#endif	//NULL

typedef struct mbuf {

  struct mbuf *next;
  unsigned int  seqnum;
  unsigned char *data;
  
}mbuf_list_t,*mbuf_list_Pt;


mbuf_list_Pt muticastBufList=NULL;

int multicastListAdd(unsigned char *data,unsigned int seq)
{
	mbuf_list_Pt pNew;
	mbuf_list_Pt pLast;
	char *load_buf;
	char *app_buf;
	
	get_param(&bParam);
	load_buf = (char*)bParam.load;

	
	pNew = (mbuf_list_Pt)malloc(sizeof(mbuf_list_t));
	
	if (pNew == NULL)
		return 0;


#if 0
	if(total_len){
			
		pNew->data = load_buf+MUTLICAST_FRAME_LENGTH*(seq-1);
		//last packet
		if(total_len<=(MUTLICAST_FRAME_LENGTH*(seq)))
			memcpy((load_buf+MUTLICAST_FRAME_LENGTH*(seq-1)),data,(total_len-MUTLICAST_FRAME_LENGTH*(seq-1)));
		else
			memcpy((load_buf+MUTLICAST_FRAME_LENGTH*(seq-1)),data,MUTLICAST_FRAME_LENGTH);

		printf("pNew->data %x seq %d\n",pNew->data,seq);
		pNew->seqnum=seq;   
		pNew->next = muticastBufList;
		muticastBufList =  pNew;
	}
#endif	
		printf("#");	
		memcpy((load_buf+MUTLICAST_FRAME_LENGTH*(seq-1)),data,MUTLICAST_FRAME_LENGTH);
		pNew->seqnum=seq;   
		pNew->next = muticastBufList;
		muticastBufList =  pNew;

	
	return 1;
}

void  multicastListFree()
{
   	mbuf_list_Pt list ;
	mbuf_list_Pt Pt_old;
       list =  muticastBufList;
	while(list !=NULL){
		Pt_old = list;
		list = list->next;
		free(Pt_old);
	} 
	muticastBufList =NULL;
	return ;
}

mbuf_list_Pt multicastListFind(unsigned int seq)
{
	mbuf_list_Pt list;
	list =  muticastBufList;

	while(list!=NULL){

		if(list->seqnum == seq){
//			printf("seq %d \n\r",seq);
			return list;
		}
		else {
			list = list->next;
		}   
	}
	
        return SEQNUM_NOT_EXIST;

}

int PrepareMulticastFrameLoadBuf(){
	
	 mbuf_list_Pt list ;
	 
	 char *load_buf;
	 unsigned int maxImageSeq;
	 int index;

	 get_param(&bParam);
	 load_buf = (char*)(bParam.load);
	 
	
	maxImageSeq = (total_len/MUTLICAST_FRAME_LENGTH)+1; 
	printf("\r\nLoad Address: 0x%08X total_len %d  maxImageSeq %d\n\r", bParam.load,total_len,maxImageSeq);
	for(index =1;index<=maxImageSeq;index++){
		if((list=multicastListFind(index)) == SEQNUM_NOT_EXIST){
			printf("image not integrity: %d lost----\n", index);
			return IMAGE_NOT_INTEGRITY;
		}	
		
	}

	if(index >maxImageSeq){
		
		return  IMAGE_INTEGRITY;
	}
}

#define MAX_PARTITION 3
partition_info partiton_info_array[MAX_PARTITION];

#ifdef ntohs
#undef ntohs
#define ntohs(x) x
#endif
#define ntohs(x) x
#define ntohl(x) x

int processMultcastUpgrade(MUTICAST_FRAME_Pt p){

	
	unsigned int seqnum;
	unsigned int packet_imagelen;
	unsigned int packet_imagecrc;
	unsigned int packet_datacrc;
	
		
	seqnum = natohl(p->dataseqnum);
	//seqnum = SWAP32(seqnum);
	
	packet_imagecrc = natohl(p->imagecrc32);
	packet_datacrc = natohl(p->datacrc32);
	//packet_imagecrc= SWAP32(packet_imagecrc);
	packet_imagelen = natohl(p->imagelength);
	//packet_imagelen= SWAP32(packet_imagelen);
	//printf("receive firmware packet image_len=0x%x image_crc=0x%x seq=%d total_crc32=0x%d total_len=0x%x type=%x len=0x%x\n\r",
	//	packet_imagelen,packet_imagecrc,seqnum,total_crc32,total_len, p->datatype, p->datalen);
	//printf("receive firmware packet image_len=0x%x image_crc=0x%x seq=%d total_crc32=0x%x total_len=0x%x type=%x len=0x%x hl=0x%x\n\r",
	//	packet_imagelen,packet_imagecrc,seqnum,total_crc32,total_len, p->hl >> 29, p->hl & 0x1fffffff, p->hl);
#if 0
	for(i=0;i<len;i++){
		if(i%0x10==0 && i!=0)
			printf("\n\r");
		printf("%02x ",pkt[i]);
	}
	printf("\n\r");
#endif
	unsigned int crc = gz_crc32(0, p->image_data, 1024);
	if(crc != packet_datacrc) {
		//printf("packet crc error: head crc32 %x calc crc32 %x. dropped!\n\r", packet_datacrc, crc);
		return 0;
	}
#if 1
	if((ntohl(p->hl) & MUP_DATATYPE_MASK<<29) == MUP_DATATYPE_INFO<<29){
#else
	if(ntohs(p->datatype)==MUP_DATATYPE_INFO)
#endif
		printf("\n\rrecv header\n\r");
		if(!multicast_frame_start){
			multicast_frame_start = 1;
		}
		
		if((total_len!=0)&&(total_crc32!=0)){
			if((packet_imagecrc ==total_crc32 ) &&(total_len == packet_imagelen )) {
				printf("\n\rthe same packet receive .....\n\r");
				multicast_frame_finish =1;
				//multicast_frame_alive  = 0;
			}
		} else {
			//printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
			//printf("=========================================================================================\n");
			total_len = packet_imagelen;
			total_crc32 = packet_imagecrc;
			printf("total_len=%x total_crc32=%x\n", total_len, total_crc32);
			printf("packet_imagelen=%x packet_imagecrc=%x\n",packet_imagelen, packet_imagecrc);
			//printf("=========================================================================================\n");
			//printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");			
		}	
	}

	 if(multicast_frame_finish){
	 	//printf("%s %d\n", __FUNCTION__, __LINE__);
	 	if(PrepareMulticastFrameLoadBuf()==IMAGE_INTEGRITY){
			printf("complete multicast data frame ....\n");
			
			multicast_frame_finish =-1;
			multicastListFree();
			return 0 ;
		} else {				
				multicast_frame_finish =0;
		}
		return 0;
	}

#if 1
	if((ntohl(p->hl) & MUP_DATATYPE_MASK<<29) == MUP_DATATYPE_INFO<<29)
#else
	if(ntohs(p->datatype) ==MUP_DATATYPE_INFO)
#endif
	{
		//printf("multicast data 1024 \n");
		int i = 0;
		partition_hdr *phdr = (partition_hdr *)p->image_data;
		multicast_frame_start =1;
		/*parse the first pkt*/
		memset(partiton_info_array, 0, sizeof(partiton_info_array));

		for(i = 0; i < MAX_PARTITION; i++) {
			if(!phdr->name[0])	/*end*/
				break;
			printf("%s recved\n", phdr->name);
			partiton_info_array[i].valid = 1;
			if(strcmp(phdr->name, "bootloader")==0)
				partiton_info_array[i].type |= PARTI_TYPE_BOOT;
			else if(strcmp(phdr->name, "kernel")==0)
				partiton_info_array[i].type |= PARTI_TYPE_IMG;
			else if(strcmp(phdr->name, "conf")==0)
				partiton_info_array[i].type |= PARTI_TYPE_CONF;
			//strcpy(partiton_info_array[i].name, phdr->name);
			partiton_info_array[i].addrstart 	= ntohl(phdr->addrstart);
			partiton_info_array[i].addrend		= ntohl(phdr->addrend);
			partiton_info_array[i].cover_flag	= phdr->cover_flag;
			phdr++;
		}
		for( ;i < MAX_PARTITION; i++)
			partiton_info_array[i].valid = 0;
	}
#if 1
	if( (ntohl(p->hl) & MUP_DATATYPE_MASK <<29) == MUP_DATATYPE_NORM<<29 ||
	    (ntohl(p->hl) & MUP_DATATYPE_MASK <<29) == MUP_DATATYPE_LAST<<29 )
#else
	if((ntohs(p->datatype)==MUP_DATATYPE_NORM)||(ntohs(p->datatype)==MUP_DATATYPE_LAST))
#endif
	{	
		if(multicastListFind(seqnum)==SEQNUM_NOT_EXIST) {
			//printf("add seq %d\n", seqnum);
			multicastListAdd(p->image_data,seqnum);				
		}
	}
	if((ntohl(p->hl) & MUP_DATATYPE_MASK<<29) == MUP_DATATYPE_NOP<<29)	/*alive pkt*/
	{
		//printf("alive packet recv\n");
		//printf("image_data[0]=%d\n", p->image_data[0]);
		if(p->image_data[0]==MUP_DATA_ALIVE_ON)
		{
			multicast_frame_alive = 1;
			multicast_frame_stop  = 0;
		}
		else if(p->image_data[0]==MUP_DATA_ALIVE_OFF)
		{
			multicast_frame_alive = 0;
			multicast_frame_stop  = 1;
		}
	}

	return 0;
	
}


#endif
void  processUDP (unsigned char *pkt, unsigned short len)
{
	register UDPpkt *p = (UDPpkt *) pkt;
	register unsigned short sum;
	fakehdr fh;
#if 0
	int i;
	for(i=0;i<len;i++){
		if(i%0x10==0 && i!=0)
			printf("\n\r");
		printf("%2x ",pkt[i]);
	}
	printf("\n\r");
#endif

	if (len >= sizeof (UDPpkt)) {
		if ((sum = p->cks)) {
			fh.sa = p->iphdr.src_ip;
			fh.da = p->iphdr.dst_ip;
			fh.pt = p->iphdr.prot;
			fh.ln = p->length;
			sum = cks_partial (&fh, sizeof fh, 0);
			sum = ~cks_partial (&p->src_port, fh.ln, sum);
		}
	//printf("sum=%x\n\r",sum);

		
		if (sum == 0) {
			if (p->src_port == BOOTP_SRV_PORT && p->dst_port == BOOTP_CLI_PORT
				&& netif.ip == 0)
				processBOOTP ((BOOTPpkt *) p);
			else if (p->dst_port == TFTP_LOCAL_PORT && netif.ip)
				processTFTP ((TFTPDpkt *) p);
#ifdef SUPPORT_MULT_UPGRADE
			else if((p->iphdr.ehdr.dst_hwadr[0]==0x01)&&(p->iphdr.ehdr.dst_hwadr[1]==0x0)&&((p->iphdr.ehdr.dst_hwadr[2]==0x5e))){
				if(((p->iphdr.dst_ip)&IP_MCAST) == IP_MCAST)
				{
					processMultcastUpgrade((MUTICAST_FRAME_Pt)p);				
				}
			}	
#endif
			
		}

		else {
			printf ("UDP checksum error\n\r");
		}
	}
	return;
}

int sendUDP (UDPpkt * p, unsigned short len, unsigned int dst_ip,
			 unsigned short src_port, unsigned short dst_port)
{
	fakehdr fh;
	register unsigned short sum;

	len += sizeof (UDPpkt) - sizeof (IPpkt);
	p->src_port = src_port;
	p->dst_port = dst_port;
	fh.ln = p->length = len;
	p->cks = 0;
	fh.sa = netif.ip;
	fh.da = dst_ip;
	fh.pt = PROTO_UDP;
	sum = cks_partial (&fh, sizeof fh, 0);
	sum = ~cks_partial (&p->src_port, len, sum);
	p->cks = sum ? sum : -1;
	return sendIP ((IPpkt *) p, len, PROTO_UDP, dst_ip);
}
