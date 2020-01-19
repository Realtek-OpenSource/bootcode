
#ifndef __ICMODEL_RING_CONTROLLER__
#define __ICMODEL_RING_CONTROLLER__



//#define MAC_LLIP_SIM 1  /* enable Low-Level IP simulation. */
//#define SAR_LLIP_SIM 1  /* enable Low-Level IP simulation. */
#define EXT_LLIP_SIM 1  /* enable Low-Level IP simulation. */



#define SAR_INTFS	8
#define MAC_RX_INTFS	2
#define MAC_TX_INTFS	5
#define EXT_INTFS	3
#define uint32 unsigned int
#define uint16 unsigned short
#define uint8	unsigned char

//#define BUFFER_SIZE 2048
struct mac_pTx
{
#ifdef __LITTLE_ENDIAN
	uint32 data_length:12;
	uint32 rsv:11;
	uint32 crc:1;
	uint32 rsv2:4;
	uint32 ls:1;
	uint32 fs:1;
	uint32 eor:1;
	uint32 own:1;
#else
	uint32 own:1;
	uint32 eor:1;
	uint32 fs:1;	
	uint32 ls:1;	
	uint32 rsv2:4;	
	uint32 crc:1;	
	uint32 rsv:11;	
	uint32 data_length:12;	
#endif

	uint32 tx_buffer_addr;


#ifdef __LITTLE_ENDIAN
	uint32 vlan_vidh:4;
	uint32 vlan_cfi:1;
	uint32 vlan_prio:3;
	uint32 vlan_vidl:8;
	uint32 tagc:1;
	uint32 rsv3:15;
#else
	uint32 rsv3:15;
	uint32 tagc:1;
	uint32 vlan_vidl:8;
	uint32 vlan_prio:3;
	uint32 vlan_cfi:1;	
	uint32 vlan_vidh:4;	
#endif

#ifdef __LITTLE_ENDIAN
	uint32 fcpu:1;
	uint32 oeor:1;
	uint32 orgAddr:30;	
#else
	uint32 orgAddr:30;
	uint32 oeor:1;
	uint32 fcpu:1;
#endif

#ifdef __LITTLE_ENDIAN
	uint32 sram_size:8;
	uint32 rsv4:8;
	uint32 sram_map_id:9;
	uint32 rsv5:7;
#else
	uint32 rsv5:7;
	uint32 sram_map_id:9;
	uint32 rsv4:8;	
	uint32 sram_size:8;
#endif
};

struct mac_vTx
{
#ifdef __LITTLE_ENDIAN
	uint32 data_length:12;
	uint32 rsv:11;
	uint32 crc:1;
	uint32 rsv2:4;
	uint32 ls:1;
	uint32 fs:1;
	uint32 eor:1;
	uint32 own:1;
#else
	uint32 own:1;
	uint32 eor:1;
	uint32 fs:1;
	uint32 ls:1;
	uint32 rsv2:4;
	uint32 crc:1;
	uint32 rsv:11;
	uint32 data_length:12;
#endif

	uint32 tx_buffer_addr;

#ifdef __LITTLE_ENDIAN
	uint32 vlan_vidh:4;
	uint32 vlan_cfi:1;
	uint32 vlan_prio:3;
	uint32 vlan_vidl:8;
	uint32 tagc:1;
	uint32 rsv3:15;
#else
	uint32 rsv3:15;
	uint32 tagc:1;
	uint32 vlan_vidl:8;	
	uint32 vlan_prio:3;	
	uint32 vlan_cfi:1;	
	uint32 vlan_vidh:4;	
#endif

#ifdef __LITTLE_ENDIAN
	uint32 fcpu:1;
	uint32 rsv4:1;
	uint32 orgAddr:30;	
#else
	uint32 orgAddr:30;
	uint32 rsv4:1;
	uint32 fcpu:1;
#endif

};

struct sar_pTx
{
#ifdef __LITTLE_ENDIAN
	uint32 data_length:12;
	uint32 rsv:4;
	uint32 clp:1;
	uint32 rsv2:1;
	uint32 pti:2;
	uint32 ethnt_offset:6;
	uint32 trlren:1;
	uint32 atmport:1;
	uint32 ls:1;
	uint32 fs:1;
	uint32 eor:1;
	uint32 own:1;
#else
	uint32 own:1;
	uint32 eor:1;
	uint32 fs:1;
	uint32 ls:1;	
	uint32 atmport:1;	
	uint32 trlren:1;	
	uint32 ethnt_offset:6;	
	uint32 pti:2;	
	uint32 rsv2:1;	
	uint32 clp:1;	
	uint32 rsv:4;	
	uint32 data_length:12;
#endif
	
	uint32 tx_buffer_addr;

#ifdef __LITTLE_ENDIAN
	uint32 rsv3:16;
	uint32 trlr:16;
#else
	uint32 trlr:16;
	uint32 rsv3:16;
#endif

#ifdef __LITTLE_ENDIAN
	uint32 fcpu:1;
	uint32 oeor:1;
	uint32 orgAddr:30;	
#else
	uint32 orgAddr:30;
	uint32 oeor:1;
	uint32 fcpu:1;
#endif

#ifdef __LITTLE_ENDIAN	
	uint32 sram_size:8;
	uint32 rsv4:8;
	uint32 sram_map_id:9;
	uint32 rsv5:7;	
#else
	uint32 rsv5:7;	
	uint32 sram_map_id:9;
	uint32 rsv4:8;	
	uint32 sram_size:8;	
#endif

};

struct sar_vTx
{

#ifdef __LITTLE_ENDIAN
	uint32 data_length:12;
	uint32 rsv:4;
	uint32 clp:1;
	uint32 rsv2:1;
	uint32 pti:2;
	uint32 ethnt_offset:6;
	uint32 trlren:1;
	uint32 atmport:1;
	uint32 ls:1;
	uint32 fs:1;
	uint32 eor:1;
	uint32 own:1;
#else
	uint32 own:1;
	uint32 eor:1;
	uint32 fs:1;	
	uint32 ls:1;
	uint32 atmport:1;	
	uint32 trlren:1;	
	uint32 ethnt_offset:6;
	uint32 pti:2;
	uint32 rsv2:1;
	uint32 clp:1;	
	uint32 rsv:4;
	uint32 data_length:12;	
#endif

	
	uint32 tx_buffer_addr;

#ifdef __LITTLE_ENDIAN
	uint32 rsv3:16;
	uint32 trlr:16;
#else
	uint32 trlr:16;
	uint32 rsv3:16;
#endif

#ifdef __LITTLE_ENDIAN
	uint32 fcpu:1;
	uint32 rsv4:1;
	uint32 orgAddr:30;	
#else
	uint32 orgAddr:30;
	uint32 rsv4:1;
	uint32 fcpu:1;
#endif
	

};


struct ext_Tx
{
#ifdef __LITTLE_ENDIAN
	uint32 data_length:12;
	uint32 rsv:18;
	uint32 eor:1;	
	uint32 own:1;	
#else
	uint32 own:1;	
	uint32 eor:1;	
	uint32 rsv:18;	
	uint32 data_length:12;	
#endif

	uint32 tx_buffer_addr;

#ifdef __LITTLE_ENDIAN
	uint32 linkid:7;
	uint32 rsv2:25;
#else
	uint32 rsv2:25;
	uint32 linkid:7;
#endif

#ifdef __LITTLE_ENDIAN
	uint32 fcpu:1;
	uint32 oeor:1;
	uint32 orgAddr:30;		
#else
	uint32 orgAddr:30;		
	uint32 oeor:1;
	uint32 fcpu:1;
#endif

	uint32 skb_header_addr;
};


struct mac_pRx
{
#ifdef __LITTLE_ENDIAN
	uint32 data_length:12;
	uint32 ipseg:1;
	uint32 tcpf:1;
	uint32 udpf:1;
	uint32 ipf:1;
	uint32 pid0:1;
	uint32 pid1:1;
	uint32 crc:1;
	uint32 runt:1;
	uint32 res:1;
	uint32 lpkt:1;
	uint32 e8023:1;
	uint32 pppoe:1;
	uint32 bar:1;
	uint32 pam:1;
	uint32 mar:1;	
	uint32 fae:1;		
	uint32 ls:1;	
	uint32 fs:1;	
	uint32 eor:1;		
	uint32 own:1;	
#else	
	uint32 own:1;	
	uint32 eor:1;		
	uint32 fs:1;	
	uint32 ls:1;	
	uint32 fae:1;		
	uint32 mar:1;	
	uint32 pam:1;
	uint32 bar:1;
	uint32 pppoe:1;
	uint32 e8023:1;
	uint32 lpkt:1;
	uint32 res:1;
	uint32 runt:1;
	uint32 crc:1;
	uint32 pid1:1;
	uint32 pid0:1;
	uint32 ipf:1;
	uint32 udpf:1;
	uint32 tcpf:1;
	uint32 ipseg:1;
	uint32 data_length:12;

#endif
	uint32 rx_buffer_addr;

#ifdef __LITTLE_ENDIAN
	uint32 vlan_vidh:4;
	uint32 vlan_cfi:1;
	uint32 vlan_prio:3;
	uint32 vlan_vidl:8;
	uint32 ava:1;
	uint32 rsv:15;
#else	
	uint32 rsv:15;
	uint32 ava:1;
	uint32 vlan_vidl:8;
	uint32 vlan_prio:3;
	uint32 vlan_cfi:1;
	uint32 vlan_vidh:4;
#endif

#ifdef __LITTLE_ENDIAN
	uint32 partial_checksum:16;
	uint32 reason:5;
	uint32 rsv1:3;
	uint32 rx_shift:8;
#else
	uint32 rx_shift:8;
	uint32 rsv1:3;
	uint32 reason:5;
	uint32 partial_checksum:16;
#endif

	uint32 skb_header_addr;

#ifdef __LITTLE_ENDIAN
	uint32 sram_map_of:8;
	uint32 rsv2:8;
	uint32 sram_map_id:9;
	uint32 rsv3:6;
	uint32 sram_en:1;
#else
	uint32 sram_en:1;
	uint32 rsv3:6;
	uint32 sram_map_id:9;
	uint32 rsv2:8;
	uint32 sram_map_of:8;
#endif

};


struct mac_vRx
{

#ifdef __LITTLE_ENDIAN
	uint32 data_length:12;
//	uint32 ipv6:1;
	uint32 ipsec:1;
	uint32 tcpf:1;
	uint32 udpf:1;
	uint32 ipf:1;
	uint32 pid0:1;
	uint32 pid1:1;
	uint32 crc:1;
	uint32 runt:1;
	uint32 res:1;
	uint32 lpkt:1;
	uint32 e8023:1;
	uint32 pppoe:1;
	uint32 bar:1;
	uint32 pam:1;
	uint32 mar:1;	
	uint32 fae:1;		
	uint32 ls:1;	
	uint32 fs:1;	
	uint32 eor:1;		
	uint32 own:1;	
#else	
	uint32 own:1;	
	uint32 eor:1;	
	uint32 fs:1;		
	uint32 ls:1;		
	uint32 fae:1;			
	uint32 mar:1;		
	uint32 pam:1;	
	uint32 bar:1;	
	uint32 pppoe:1;	
	uint32 e8023:1;	
	uint32 lpkt:1;	
	uint32 res:1;	
	uint32 runt:1;
	uint32 crc:1;	
	uint32 pid1:1;
	uint32 pid0:1;
	uint32 ipf:1;	
	uint32 udpf:1;	
	uint32 tcpf:1;	
	uint32 ipsec:1;	
	uint32 data_length:12;	
#endif	


	uint32 rx_buffer_addr;

#ifdef __LITTLE_ENDIAN
	uint32 vlan_vidh:4;
	uint32 vlan_cfi:1;
	uint32 vlan_prio:3;
	uint32 vlan_vidl:8;
	uint32 ava:1;
	uint32 rsv:15;
#else
	uint32 rsv:15;
	uint32 ava:1;
	uint32 vlan_vidl:8;	
	uint32 vlan_prio:3;	
	uint32 vlan_cfi:1;	
	uint32 vlan_vidh:4;	
#endif


#ifdef __LITTLE_ENDIAN
	uint32 partial_checksum:16;
	uint32 reason:5;
	uint32 rsv1:3;
	uint32 rx_shift:8;
#else
	uint32 rx_shift:8;
	uint32 rsv1:3;
	uint32 reason:5;	
	uint32 partial_checksum:16;	
#endif


	uint32 skb_header_addr;

};


struct sar_pRx
{
#ifdef __LITTLE_ENDIAN
	uint32 data_length:12;
	uint32 ipv6:1;
	uint32 l4csmf:1;
	uint32 mar:1;
	uint32 bar:1;
	uint32 clp:1;
	uint32 pti:3;
	uint32 wii:1;
	uint32 frgi:1;
	uint32 ppi:2;
	uint32 ipcerr:1;
	uint32 lenerr:1;
	uint32 crc32err:1;
	uint32 atmport:1;
	uint32 ls:1;
	uint32 fs:1;
	uint32 eor:1;		
	uint32 own:1;	
#else
	uint32 own:1;	
	uint32 eor:1;	
	uint32 fs:1;	
	uint32 ls:1;	
	uint32 atmport:1;	
	uint32 crc32err:1;	
	uint32 lenerr:1;	
	uint32 ipcerr:1;	
	uint32 ppi:2;	
	uint32 frgi:1;	
	uint32 wii:1;	
	uint32 pti:3;	
	uint32 clp:1;	
	uint32 bar:1;	
	uint32 mar:1;	
	uint32 l4csmf:1;	
	uint32 ipv6:1;	
	uint32 data_length:12;	
#endif

	uint32 rx_buffer_addr;

#ifdef __LITTLE_ENDIAN
	uint32 bpc:16;
	uint32 trlr:16;
#else
	uint32 trlr:16;
	uint32 bpc:16;
#endif

#ifdef __LITTLE_ENDIAN
	uint32 tucacc:16;
	uint32 reason:5;
	uint32 rsv:3;
	uint32 rx_shift:8;
#else
	uint32 rx_shift:8;
	uint32 rsv:3;
	uint32 reason:5;	
	uint32 tucacc:16;	
#endif

	uint32 skb_header_addr;

#ifdef __LITTLE_ENDIAN
	uint32 sram_map_of:8;
	uint32 rsv1:8;
	uint32 sram_map_id:9;
	uint32 rsv2:6;	
	uint32 sram_en:1;
#else
	uint32 sram_en:1;
	uint32 rsv2:6;		
	uint32 sram_map_id:9;	
	uint32 rsv1:8;	
	uint32 sram_map_of:8;	
#endif


};


struct sar_vRx
{
#ifdef __LITTLE_ENDIAN
	uint32 data_length:12;
	uint32 ipv6:1;
	uint32 l4csmf:1;
	uint32 mar:1;
	uint32 bar:1;
	uint32 clp:1;
	uint32 pti:3;
	uint32 wii:1;
	uint32 frgi:1;
	uint32 ppi:2;
	uint32 ipcerr:1;
	uint32 lenerr:1;
	uint32 crc32err:1;
	uint32 atmport:1;
	uint32 ls:1;
	uint32 fs:1;
	uint32 eor:1;		
	uint32 own:1;	
#else
	uint32 own:1;	
	uint32 eor:1;	
	uint32 fs:1;	
	uint32 ls:1;	
	uint32 atmport:1;	
	uint32 crc32err:1;	
	uint32 lenerr:1;	
	uint32 ipcerr:1;	
	uint32 ppi:2;	
	uint32 frgi:1;	
	uint32 wii:1;	
	uint32 pti:3;	
	uint32 clp:1;	
	uint32 bar:1;	
	uint32 mar:1;	
	uint32 l4csmf:1;	
	uint32 ipv6:1;	
	uint32 data_length:12;	
#endif

	uint32 rx_buffer_addr;

#ifdef __LITTLE_ENDIAN
	uint32 bpc:16;
	uint32 trlr:16;
#else
	uint32 trlr:16;
	uint32 bpc:16;
#endif

#ifdef __LITTLE_ENDIAN
	uint32 tucacc:16;
	uint32 reason:5;
	uint32 rsv:3;
	uint32 rx_shift:8;
#else
	uint32 rx_shift:8;
	uint32 rsv:3;
	uint32 reason:5;	
	uint32 tucacc:16;	
#endif

	uint32 skb_header_addr;

};


struct ext_Rx
{
#ifdef __LITTLE_ENDIAN
	uint32 data_length:12;
	uint32 rsv:18;
	uint32 eor:1;	
	uint32 own:1;	
#else
	uint32 own:1;	
	uint32 eor:1;	
	uint32 rsv:18;	
	uint32 data_length:12;	
#endif

	uint32 rx_buffer_addr;

#ifdef __LITTLE_ENDIAN
	uint32 linkid:7;
	uint32 rsv2:9;
	uint32 reason:5;
	uint32 rsv3:3;	
	uint32 rx_shift:8;	
#else
	uint32 rx_shift:8;	
	uint32 rsv3:3;	
	uint32 reason:5;	
	uint32 rsv2:9;	
	uint32 linkid:7;	
#endif

	uint32 skb_header_addr;
};


struct sp_pRx
{
#ifdef __LITTLE_ENDIAN
//	uint32 data_length:12;
	uint32 rsv4:12;
	uint32 rsv:18;
	uint32 eor:1;		
	uint32 own:1;	
#else
	uint32 own:1;	
	uint32 eor:1;		
	uint32 rsv:18;
	uint32 rsv4:12;	
//	uint32 data_length:12;	
#endif

	uint32 rx_buffer_addr;

#if 0
#ifdef __LITTLE_ENDIAN
	uint32 linkid:7;
	uint32 rsv2:17;
//	uint32 rx_shift:8;
	uint32 rsv5:8;	
#else
	uint32 rsv5:8;	
//	uint32 rx_shift:8;
	uint32 rsv2:17;
	uint32 linkid:7;
#endif
#endif

	uint32 skb_header_addr;

#ifdef __LITTLE_ENDIAN
	uint32 sram_map_of:8;
	uint32 rsv1:8;
	uint32 sram_map_id:9;
	uint32 rsv3:6;	
	uint32 sram_en:1;
#else
	uint32 sram_en:1;
	uint32 rsv3:6;	
	uint32 sram_map_id:9;
	uint32 rsv1:8;
	uint32 sram_map_of:8;			
#endif

};

#ifdef RTL867X_MODEL_USER
#else
struct sk_buff2
{
	unsigned char *head;
	unsigned char *data;
	unsigned char *tail;	
	unsigned char *end;	
	unsigned int data_len;
};
#endif



#endif

