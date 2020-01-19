/*
 *      Header file of MIB
 *      Authors: David Hsu	<davidhsu@realtek.com.tw>
 *      Authors: Dick Tam	<dicktam@realtek.com.tw>
 *
 */


#ifndef INCLUDE_MIB_H
#define INCLUDE_MIB_H

//#ifdef __mips__
//#include <linux/config.h>
//#else
//#include "../../../uClibc-0.9.15/include/linux/autoconf.h"
//#endif

#ifndef WIN32
#define __PACK__				__attribute__ ((packed))
#else
#define __PACK__
#endif

#ifdef WIN32
#pragma pack()
#endif

/*
 * Flash File System 
 */

typedef enum { UNKNOWN_SETTING=0, RUNNING_SETTING=1, HW_SETTING=2, DEFAULT_SETTING=4, CURRENT_SETTING=8 } CONFIG_DATA_T;
#define SIGNATURE_LEN				8
#define HS_CONF_SETTING_SIGNATURE_TAG		((char *)"ADSL-HS-")
#define DS_CONF_SETTING_SIGNATURE_TAG		((char *)"ADSL-DS-")
#define CS_CONF_SETTING_SIGNATURE_TAG		((char *)"ADSL-CS-")
#define WEB_SIGNATURE_TAG			((char *)"ADSL-WEB")
#define FLASH_FILE_SYSTEM_VERSION		1

/* File header */
typedef struct param_header {
	unsigned char signature[SIGNATURE_LEN] __PACK__;
	unsigned char version __PACK__;
	unsigned char checksum __PACK__;
	unsigned int len __PACK__;
} PARAM_HEADER_T, *PARAM_HEADER_Tp;

/* Firmware image header */
typedef struct _header_ {
 // Kao
	unsigned long signature;
	unsigned long startAddr;
	unsigned long len;
} IMG_HEADER_T, *IMG_HEADER_Tp;

#define FIRMWARE_MAGIC_NUMBER		0xa3d275e9
#define FIRMWARE_PARAM_SIZE		0x10
#define DST_IMAGE_ADDR			0x80000000

/* scramble saved configuration data */
#define ENCODE_DATA(data,len)
#define DECODE_DATA(data,len)

/*
#define ENCODE_DATA(data,len) { \
	int i; \
	for (i=0; i<len; i++) \
		data[i] = ~ ( data[i] + 0x38); \
}

#define DECODE_DATA(data,len) { \
	int i; \
	for (i=0; i<len; i++) \
		data[i] = ~data[i] - 0x38;	\
}
*/

/* Do checksum and verification for configuration data */
#ifndef WIN32
static inline unsigned char CHECKSUM(unsigned char *data, unsigned int len)
#else
__inline unsigned char CHECKSUM(unsigned char *data, unsigned int len)
#endif
{
	unsigned int i;
	unsigned char sum=0;

	for (i=0; i<len; i++) {
		sum += data[i];
	}
	
	sum = ~sum + 1;
	return sum;
}










/*
 * Webpage gzip/unzip
 */
#define GZIP_MAX_NAME_LEN		60
typedef struct file_entry {
	char name[GZIP_MAX_NAME_LEN];
	unsigned long size;
} FILE_ENTRY_T, *FILE_ENTRY_Tp;


#define DWORD_SWAP(v) ( (((v&0xff)<<24)&0xff000000) | \
						((((v>>8)&0xff)<<16)&0xff0000) | \
						((((v>>16)&0xff)<<8)&0xff00) | \
						(((v>>24)&0xff)&0xff) )
#define WORD_SWAP(v) ((unsigned short)(((v>>8)&0xff) | ((v<<8)&0xff00)))










/*
 * ADSL Router MIB ID
 */
#define CS_ENTRY_ID					0						

#define MIB_ADSL_LAN_IP					CS_ENTRY_ID + 1
#define MIB_ADSL_LAN_SUBNET				CS_ENTRY_ID + 2
#define MIB_ADSL_LAN_GATEWAY				CS_ENTRY_ID + 3
#define MIB_ADSL_LAN_MAC_ADDR				CS_ENTRY_ID + 4
#define MIB_ADSL_LAN_DHCP				CS_ENTRY_ID + 5
#define MIB_ADSL_LAN_CLIENT_START			CS_ENTRY_ID + 6
#define MIB_ADSL_LAN_CLIENT_END				CS_ENTRY_ID + 7
#define MIB_ADSL_LAN_DHCP_LEASE				CS_ENTRY_ID + 8
#define MIB_ADSL_LAN_DHCP_DOMAIN			CS_ENTRY_ID + 9
#define MIB_ADSL_LAN_RIP				CS_ENTRY_ID + 10
#define MIB_ADSL_LAN_AUTOSEARCH				CS_ENTRY_ID + 11

#define MIB_ADSL_WAN_DNS_MODE				CS_ENTRY_ID + 19
#define MIB_ADSL_WAN_DNS1				CS_ENTRY_ID + 20
#define MIB_ADSL_WAN_DNS2				CS_ENTRY_ID + 21
#define MIB_ADSL_WAN_DNS3				CS_ENTRY_ID + 22
#define MIB_ADSL_CONNECTION_MODE			CS_ENTRY_ID + 24
#define MIB_ADSL_ENCAP_MODE				CS_ENTRY_ID + 25

#define MIB_ADSL_MODE					CS_ENTRY_ID + 26
#define MIB_ADSL_OLR					CS_ENTRY_ID + 27

#define MIB_RIP_ENABLE					CS_ENTRY_ID + 28
#define MIB_RIP_INTERFACE				CS_ENTRY_ID + 29
#define MIB_RIP_VERSION					CS_ENTRY_ID + 30

#define MIB_IPF_OUT_ACTION				CS_ENTRY_ID + 32
#define MIB_IPF_IN_ACTION				CS_ENTRY_ID + 33
#define MIB_MACF_OUT_ACTION				CS_ENTRY_ID + 34
#define MIB_MACF_IN_ACTION				CS_ENTRY_ID + 35
#define MIB_PORT_FW_ENABLE				CS_ENTRY_ID + 36
#define MIB_DMZ_ENABLE					CS_ENTRY_ID + 37
#define MIB_DMZ_IP					CS_ENTRY_ID + 38
	
#define MIB_USER_NAME					CS_ENTRY_ID + 41
#define MIB_USER_PASSWORD				CS_ENTRY_ID + 42
#define MIB_DEVICE_TYPE					CS_ENTRY_ID + 43
#define MIB_INIT_LINE					CS_ENTRY_ID + 44
#define MIB_INIT_SCRIPT					CS_ENTRY_ID + 45

#define MIB_SNMP_SYS_DESCR				CS_ENTRY_ID + 51
#define MIB_SNMP_SYS_OID				CS_ENTRY_ID + 52
#define MIB_SNMP_SYS_CONTACT				CS_ENTRY_ID + 53
#define MIB_SNMP_SYS_NAME				CS_ENTRY_ID + 54
#define MIB_SNMP_SYS_LOCATION				CS_ENTRY_ID + 55
#define MIB_SNMP_TRAP_IP				CS_ENTRY_ID + 56
#define MIB_SNMP_COMM_RO				CS_ENTRY_ID + 57
#define MIB_SNMP_COMM_RW				CS_ENTRY_ID + 58

#define MIB_ATM_LOOPBACK				CS_ENTRY_ID + 59
#define MIB_ATM_MODE					CS_ENTRY_ID + 60
#define MIB_ATM_VC_SWITCH				CS_ENTRY_ID + 61
#define MIB_ATM_MAC1					CS_ENTRY_ID + 62
#define MIB_ATM_MAC2					CS_ENTRY_ID + 63
#define MIB_ATM_VC_AUTOSEARCH				CS_ENTRY_ID + 64
// Kao
#define MIB_BRCTL_AGEINGTIME				CS_ENTRY_ID + 71
#define MIB_BRCTL_STP					CS_ENTRY_ID + 72

#ifdef CONFIG_RE8305
#define MIB_MPMODE					CS_ENTRY_ID + 73
#endif

#define MIB_IGMP_PROXY					CS_ENTRY_ID + 77
#define MIB_IGMP_PROXY_ITF				CS_ENTRY_ID + 78
#define MIB_IPPT_ITF					CS_ENTRY_ID + 79
#define MIB_IPPT_LEASE					CS_ENTRY_ID + 80
#define MIB_IPPT_LANACC					CS_ENTRY_ID + 81
#define MIB_SPC_ENABLE					CS_ENTRY_ID + 82
#define MIB_SPC_IPTYPE					CS_ENTRY_ID + 83
#define MIB_ACL_CAPABILITY				CS_ENTRY_ID + 84
#define MIB_ADSL_WAN_DHCPS				CS_ENTRY_ID + 85
#define MIB_DHCP_MODE					CS_ENTRY_ID + 86

#ifdef WLAN_SUPPORT
// WLAN MIB id
#define MIB_WLAN_SSID					CS_ENTRY_ID + 101
#define MIB_WLAN_CHAN_NUM				CS_ENTRY_ID + 102
#define MIB_WLAN_WEP					CS_ENTRY_ID + 103
#define MIB_WLAN_WEP64_KEY1				CS_ENTRY_ID + 104
#define MIB_WLAN_WEP64_KEY2				CS_ENTRY_ID + 105
#define MIB_WLAN_WEP64_KEY3				CS_ENTRY_ID + 106
#define MIB_WLAN_WEP64_KEY4				CS_ENTRY_ID + 107
#define MIB_WLAN_WEP128_KEY1				CS_ENTRY_ID + 108
#define MIB_WLAN_WEP128_KEY2				CS_ENTRY_ID + 109
#define MIB_WLAN_WEP128_KEY3				CS_ENTRY_ID + 110
#define MIB_WLAN_WEP128_KEY4				CS_ENTRY_ID + 111
#define MIB_WLAN_WEP_KEY_TYPE				CS_ENTRY_ID + 112
#define MIB_WLAN_WEP_DEFAULT_KEY			CS_ENTRY_ID + 113
#define MIB_WLAN_FRAG_THRESHOLD				CS_ENTRY_ID + 114
#define MIB_WLAN_SUPPORTED_RATE				CS_ENTRY_ID + 115
#define MIB_WLAN_BEACON_INTERVAL			CS_ENTRY_ID + 116
#define MIB_WLAN_PREAMBLE_TYPE				CS_ENTRY_ID + 117
#define MIB_WLAN_BASIC_RATE				CS_ENTRY_ID + 118
#define MIB_WLAN_RTS_THRESHOLD				CS_ENTRY_ID + 119
#define MIB_WLAN_AUTH_TYPE				CS_ENTRY_ID + 120
#define MIB_WLAN_HIDDEN_SSID				CS_ENTRY_ID + 121
#define MIB_WLAN_DISABLED				CS_ENTRY_ID + 122
#define MIB_ALIAS_NAME					CS_ENTRY_ID + 123
// Added by Mason Yu for TxPower
#define MIB_TX_POWER					CS_ENTRY_ID + 124 
 
                                        		
#ifdef WLAN_WPA
#define MIB_WLAN_ENCRYPT				CS_ENTRY_ID + 126
#define MIB_WLAN_ENABLE_SUPP_NONWPA			CS_ENTRY_ID + 127
#define MIB_WLAN_SUPP_NONWPA				CS_ENTRY_ID + 128
#define MIB_WLAN_WPA_AUTH				CS_ENTRY_ID + 129
#define MIB_WLAN_WPA_CIPHER_SUITE			CS_ENTRY_ID + 130
#define MIB_WLAN_WPA_PSK				CS_ENTRY_ID + 131
#define MIB_WLAN_WPA_GROUP_REKEY_TIME			CS_ENTRY_ID + 132
#define MIB_WLAN_RS_IP					CS_ENTRY_ID + 133
#define MIB_WLAN_RS_PORT				CS_ENTRY_ID + 134
#define MIB_WLAN_RS_PASSWORD				CS_ENTRY_ID + 135
#define MIB_WLAN_ENABLE_1X				CS_ENTRY_ID + 136
#define MIB_WLAN_WPA_PSK_FORMAT				CS_ENTRY_ID + 137
#define MIB_WLAN_WPA2_PRE_AUTH				CS_ENTRY_ID + 138
#define MIB_WLAN_WPA2_CIPHER_SUITE			CS_ENTRY_ID + 139
#endif                                  		
                                        		
#define MIB_WLAN_INACTIVITY_TIME			CS_ENTRY_ID + 140
#define MIB_WLAN_RATE_ADAPTIVE_ENABLED			CS_ENTRY_ID + 141
                                        		
#ifdef WLAN_ACL
// access control MIB id
#define MIB_WLAN_AC_ENABLED				CS_ENTRY_ID + 142
#define MIB_WLAN_AC_NUM					CS_ENTRY_ID + 143
#define MIB_WLAN_AC_ADDR				CS_ENTRY_ID + 144
#define MIB_WLAN_AC_ADDR_ADD				CS_ENTRY_ID + 145
#define MIB_WLAN_AC_ADDR_DEL				CS_ENTRY_ID + 146
#define MIB_WLAN_AC_ADDR_DELALL				CS_ENTRY_ID + 147
#endif

#define MIB_WLAN_DTIM_PERIOD				CS_ENTRY_ID + 150
#define MIB_WLAN_MODE					CS_ENTRY_ID + 151
#define MIB_WLAN_NETWORK_TYPE				CS_ENTRY_ID + 152
#define MIB_WLAN_DEFAULT_SSID				CS_ENTRY_ID + 153	// used while configured as Ad-hoc and no any other Ad-hoc could be joined
								// it will use this default SSID to start BSS
#ifdef WLAN_WPA
#define MIB_WLAN_ACCOUNT_RS_ENABLED			CS_ENTRY_ID + 160
#define MIB_WLAN_ACCOUNT_RS_IP				CS_ENTRY_ID + 161
#define MIB_WLAN_ACCOUNT_RS_PORT			CS_ENTRY_ID + 162
#define MIB_WLAN_ACCOUNT_RS_PASSWORD			CS_ENTRY_ID + 163
#define MIB_WLAN_ACCOUNT_UPDATE_ENABLED			CS_ENTRY_ID + 164
#define MIB_WLAN_ACCOUNT_UPDATE_DELAY			CS_ENTRY_ID + 165
#define MIB_WLAN_ENABLE_MAC_AUTH			CS_ENTRY_ID + 166
#define MIB_WLAN_RS_RETRY				CS_ENTRY_ID + 167
#define MIB_WLAN_RS_INTERVAL_TIME			CS_ENTRY_ID + 168
#define MIB_WLAN_ACCOUNT_RS_RETRY			CS_ENTRY_ID + 169
#define MIB_WLAN_ACCOUNT_RS_INTERVAL_TIME		CS_ENTRY_ID + 170
#endif

#ifdef WLAN_IAPP
#define MIB_WLAN_IAPP_DISABLED				CS_ENTRY_ID + 180
#endif
                                        		
#ifdef WLAN_WDS
#define MIB_WLAN_WDS_ENABLED				CS_ENTRY_ID + 181
#define MIB_WLAN_WDS_NUM				CS_ENTRY_ID + 182
#define MIB_WLAN_WDS					CS_ENTRY_ID + 183
#define MIB_WLAN_WDS_ADD				CS_ENTRY_ID + 184
#define MIB_WLAN_WDS_DEL				CS_ENTRY_ID + 185
#define MIB_WLAN_WDS_DELALL				CS_ENTRY_ID + 186
#define MIB_WLAN_WDS_WEP				CS_ENTRY_ID + 187
#define MIB_WLAN_WDS_WEP64_KEY1				CS_ENTRY_ID + 188
#define MIB_WLAN_WDS_WEP64_KEY2				CS_ENTRY_ID + 189
#define MIB_WLAN_WDS_WEP64_KEY3				CS_ENTRY_ID + 190
#define MIB_WLAN_WDS_WEP64_KEY4				CS_ENTRY_ID + 191
#define MIB_WLAN_WDS_WEP128_KEY1			CS_ENTRY_ID + 192
#define MIB_WLAN_WDS_WEP128_KEY2			CS_ENTRY_ID + 193
#define MIB_WLAN_WDS_WEP128_KEY3			CS_ENTRY_ID + 194
#define MIB_WLAN_WDS_WEP128_KEY4			CS_ENTRY_ID + 195
#define MIB_WLAN_WDS_WEP_KEY_TYPE			CS_ENTRY_ID + 196
#define MIB_WLAN_WDS_WEP_DEFAULT_KEY			CS_ENTRY_ID + 197
#endif

#ifdef WLAN_8185AG
#define MIB_WLAN_BAND					CS_ENTRY_ID + 198
#define MIB_WLAN_FIX_RATE				CS_ENTRY_ID + 199
#endif

#define MIB_WLAN_PRIVACY_CHECK				CS_ENTRY_ID + 200
#define MIB_WLAN_BLOCK_RELAY				CS_ENTRY_ID + 201
#define MIB_NAT25_MAC_CLONE				CS_ENTRY_ID + 202

#endif

// Added by Mason Yu for write superUser into Current Setting
#define MIB_SUSER_NAME					CS_ENTRY_ID + 203
#define MIB_SUSER_PASSWORD				CS_ENTRY_ID + 204
#define MIB_ADSL_TONE					CS_ENTRY_ID + 205


#define HS_ENTRY_ID					250

#define MIB_SUPER_NAME					HS_ENTRY_ID + 1
#define MIB_SUPER_PASSWORD				HS_ENTRY_ID + 2
#define MIB_BOOT_MODE					HS_ENTRY_ID + 3
#define MIB_ELAN_MAC_ADDR				HS_ENTRY_ID + 4
#define MIB_WLAN_MAC_ADDR				HS_ENTRY_ID + 5
#if WLAN_SUPPORT

#define MIB_HW_REG_DOMAIN				HS_ENTRY_ID + 6
#define MIB_HW_RF_TYPE					HS_ENTRY_ID + 7
#ifndef WLAN_8185AG
#define MIB_HW_TX_POWER					HS_ENTRY_ID + 8
#else
#define MIB_HW_TX_POWER_CCK				HS_ENTRY_ID + 9
#define MIB_HW_TX_POWER_OFDM				HS_ENTRY_ID + 10
#endif
#define MIB_HW_ANT_DIVERSITY				HS_ENTRY_ID + 11
#define MIB_HW_TX_ANT					HS_ENTRY_ID + 12
#define MIB_HW_CS_THRESHOLD				HS_ENTRY_ID + 13
#define MIB_HW_CCA_MODE					HS_ENTRY_ID + 14
#define MIB_HW_PHY_TYPE					HS_ENTRY_ID + 15
#define MIB_HW_LED_TYPE					HS_ENTRY_ID + 16

#endif // of WLAN_SUPPORT

#define MIB_BYTE_TEST					HS_ENTRY_ID + 17
#define MIB_WORD_TEST					HS_ENTRY_ID + 18
#define MIB_DWORD_TEST					HS_ENTRY_ID + 19
#define MIB_INTERGER_TEST1				HS_ENTRY_ID + 20
#define MIB_INTERGER_TEST2				HS_ENTRY_ID + 21



#define CHAIN_ENTRY_TBL_ID				300

#define MIB_IP_PORT_FILTER_TBL				CHAIN_ENTRY_TBL_ID + 1
#define MIB_MAC_FILTER_TBL				CHAIN_ENTRY_TBL_ID + 2
#define MIB_PORT_FW_TBL					CHAIN_ENTRY_TBL_ID + 3
#define MIB_ATM_VC_TBL					CHAIN_ENTRY_TBL_ID + 4
#define MIB_IP_ROUTE_TBL				CHAIN_ENTRY_TBL_ID + 5
#define MIB_ACL_IP_TBL					CHAIN_ENTRY_TBL_ID + 6
#ifdef WLAN_SUPPORT
#ifdef WLAN_ACL
#define MIB_WLAN_AC_TBL					CHAIN_ENTRY_TBL_ID + 7
#endif
#endif
#ifdef CONFIG_RE8305
#define MIB_SW_PORT_TBL					CHAIN_ENTRY_TBL_ID + 8
#define MIB_VLAN_TBL					CHAIN_ENTRY_TBL_ID + 9
#define MIB_IP_QOS_TBL					CHAIN_ENTRY_TBL_ID + 10
#endif
#define MIB_ACC_TBL					CHAIN_ENTRY_TBL_ID + 11


/*
 * MIB value and constant
 */
#define MAX_NAME_LEN					30
#define MAX_FILTER_NUM					20
#define MAX_VC_NUM					8
#define MAX_PPP_NUM					8
#define MAX_IFINDEX					7
#define COMMENT_LEN					20
#define IP_ADDR_LEN					4
#define MAC_ADDR_LEN					6
#define SNMP_STRING_LEN					64

#ifdef CONFIG_RE8305
#define MAX_QOS_RULE					16
#define SW_PORT_NUM					4
#define VLAN_NUM					4
#endif

#ifdef WLAN_SUPPORT

#define MAX_SSID_LEN					33
#define WEP64_KEY_LEN					5
#define WEP128_KEY_LEN					13
#define MAX_CHAN_NUM					14

#ifdef WLAN_WPA
#define MAX_PSK_LEN					64
#define MAX_RS_PASS_LEN					32
#endif

#define TX_RATE_1M					0x01
#define TX_RATE_2M					0x02
#define TX_RATE_5M					0x04
#define TX_RATE_11M					0x08
                                        		
#ifdef WLAN_8185AG                      		
#define TX_RATE_6M					0x10
#define TX_RATE_9M					0x20
#define TX_RATE_12M					0x40
#define TX_RATE_18M					0x80
#define TX_RATE_24M					0x100
#define TX_RATE_36M					0x200
#define TX_RATE_48M					0x400
#define TX_RATE_54M					0x800
#endif                                  		
                                        		
#define MAX_WLAN_AC_NUM					20

#define MAXFNAME					60

#ifdef WLAN_WDS
#define MAX_WDS_NUM					8
#endif

#define MAX_STA_NUM					64	// max support sta number

/* flag of sta info */
#define STA_INFO_FLAG_AUTH_OPEN     			0x01
#define STA_INFO_FLAG_AUTH_WEP      			0x02
#define STA_INFO_FLAG_ASOC          			0x04
#define STA_INFO_FLAG_ASLEEP        			0x08

#endif // of WLAN_SUPPORT

#define ENCAP_VCMUX					0
#define ENCAP_LLC					1

#define ADSL_BR1483		0
#define ADSL_MER1483		1
#define ADSL_PPPoE		2
#define ADSL_PPPoA		3
#define ADSL_RT1483		4

/*-- Macro declarations --*/
#define VC_INDEX(x)					(x & 0x0f)
#define PPP_INDEX(x)					(x  >> 4)

// Added by Mason Yu for ADSL Tone
#define MAX_ADSL_TONE		65           // Added by Mason Yu for correct Tone Mib Type

typedef enum { DHCP_DISABLED=0, DHCP_CLIENT=1, DHCP_SERVER=2, PPPOOE=3 } DHCP_T;
typedef enum { DNS_AUTO=0, DNS_MANUAL } DNS_TYPE_T;
typedef enum { CONTINUOUS=0, CONNECT_ON_DEMAND, MANUAL } PPP_CONNECT_TYPE_T;
typedef enum { PPP_AUTO=0, PPP_PAP, PPP_CHAP } PPP_AUTH_T;
//12/23/05' hrchen, PROTO_TCP is already defined, comment out for fixing compiler error
//typedef enum { PROTO_NONE=0, PROTO_TCP=1, PROTO_UDP=2, PROTO_ICMP=3, PROTO_UDPTCP } PROTO_TYPE_T;
typedef enum { DIR_OUT=0, DIR_IN } DIR_T;
typedef enum { DHCP_LAN_NONE=0, DHCP_LAN_RELAY=1, DHCP_LAN_SERVER=2 } DHCP_TYPE_T;
typedef enum { BOOT_LAST=0, BOOT_DEFAULT=1, BOOT_UPGRADE=2 } BOOT_TYPE_T;
typedef enum { ACC_BRIDGED=0, ACC_MER, ACC_PPPOE, ACC_PPPOA, ACC_ROUTED } REMOTE_ACCESS_T;
typedef enum { ATMQOS_UBR=0, ATMQOS_CBR, ATMQOS_VBR_RT, ATMQOS_VBR_NRT } ATM_QOS_T;
#ifdef CONFIG_RE8305
typedef enum { MP_NONE=0, MP_PORT_MAP=1, MP_VLAN=2, MP_IPQOS=3, MP_IGMPSNOOP=4 } MP_TYPE_T;
typedef enum { LINK_10HALF=0, LINK_10FULL, LINK_100HALF, LINK_100FULL, LINK_AUTO } LINK_TYPE_T;
#endif

#ifdef WLAN_SUPPORT

#ifdef WLAN_WPA
typedef enum { ENCRYPT_DISABLED=0, ENCRYPT_WEP=1, ENCRYPT_WPA=2, ENCRYPT_WPA2=4, ENCRYPT_WPA2_MIXED=6 } ENCRYPT_T;
typedef enum { SUPP_NONWPA_NONE=0,SUPP_NONWPA_WEP=1,SUPP_NONWPA_1X=2} SUPP_NONWAP_T;
typedef enum { WPA_AUTH_AUTO=1, WPA_AUTH_PSK=2 } WPA_AUTH_T;
typedef enum { WPA_CIPHER_TKIP=1, WPA_CIPHER_AES=2 } WPA_CIPHER_T;
#endif

typedef enum { WEP_DISABLED=0, WEP64=1, WEP128=2 } WEP_T;
typedef enum { KEY_ASCII=0, KEY_HEX } KEY_TYPE_T;
typedef enum { LONG_PREAMBLE=0, SHORT_PREAMBLE=1 } PREAMBLE_T;
typedef enum { AUTH_OPEN=0, AUTH_SHARED, AUTH_BOTH } AUTH_TYPE_T;
typedef enum { RF_INTERSIL=1, RF_RFMD=2, RF_PHILIP=3, RF_MAXIM=4, RF_GCT=5,
#ifdef WLAN_8185AG
 		RF_MAXIM_AG=6, RF_ZEBRA=7,
#endif
	     } RF_TYPE_T;
typedef enum { AP_MODE=0, CLIENT_MODE=1 } WLAN_MODE_T;
typedef enum { INFRASTRUCTURE=0, ADHOC=1 } NETWORK_TYPE_T;

#ifdef WLAN_8185AG
typedef enum { BAND_11B=1, BAND_11G=2, BAND_11BG=3, BAND_11A=4 } BAND_TYPE_T;
#endif

/* WLAN sta info structure */
typedef struct wlan_sta_info {
        unsigned short  aid;
        unsigned char   addr[6];
        unsigned long   tx_packets;
        unsigned long   rx_packets;
	unsigned long	expired_time;  // 10 mini-sec
	unsigned short  flag;
        unsigned char   txOperaRates;
} WLAN_STA_INFO_T, *WLAN_STA_INFO_Tp;

typedef struct macfilter_type {
	unsigned char macAddr[6] __PACK__;
	unsigned char comment[COMMENT_LEN] __PACK__;
} MACFILTER_T, *MACFILTER_Tp;

#ifdef WLAN_WDS
typedef MACFILTER_T WDS_T;
typedef MACFILTER_Tp WDS_Tp;
#endif


#endif // of WLAN_SUPPORT
/*
 * MIB struct
 */
typedef struct config_setting {
	// TCP/IP stuffs
	unsigned char wanmode; // Magician: ADSL/ETH Wan switch
	unsigned char ipAddr[IP_ADDR_LEN] __PACK__;
	unsigned char subnetMask[IP_ADDR_LEN] __PACK__;
	unsigned char defaultGateway[IP_ADDR_LEN] __PACK__;
	unsigned char dhcp __PACK__; // DHCP flag, 0 - disabled, 1 - client, 2 - server
	unsigned char rip __PACK__; // RIP flag, 0 - disabled, 1 - enabled
	unsigned char dhcpClientStart __PACK__; // DHCP client start range
	unsigned char dhcpClientEnd __PACK__; // DHCP client end range
	unsigned int dhcpLTime __PACK__; // DHCP server max lease time in seconds
	unsigned char dhcpDomain[MAX_NAME_LEN] __PACK__; // DHCP option Domain Name
	unsigned char lanAutoSearch __PACK__;		// the LAN ip auto-search

	// web server account
	unsigned char userName[MAX_NAME_LEN] __PACK__; // user name
	unsigned char userPassword[MAX_NAME_LEN] __PACK__; // user assword
	unsigned char deviceType __PACK__; // bridge: 0 or router: 1
	unsigned char initLine __PACK__; // init adsl line on startup
	unsigned char initScript __PACK__; // init system with user configuration on startup

	unsigned char wanDhcp __PACK__; // DHCP flag for WAN port, 0 - disabled, 1 - DHCP client
	unsigned char wanIpAddr[IP_ADDR_LEN] __PACK__;
	unsigned char wanSubnetMask[IP_ADDR_LEN] __PACK__;
	unsigned char wanDefaultGateway[IP_ADDR_LEN] __PACK__;
	unsigned char pppUserName[MAX_NAME_LEN] __PACK__;
	unsigned char pppPassword[MAX_NAME_LEN] __PACK__;
	DNS_TYPE_T dnsMode __PACK__;
	unsigned char dns1[IP_ADDR_LEN], dns2[IP_ADDR_LEN], dns3[IP_ADDR_LEN] __PACK__;
	unsigned char dhcps[IP_ADDR_LEN] __PACK__;
	unsigned char dhcpMode __PACK__; // 0 - None, 1 - DHCP Relay, 2 - DHCP Server
	unsigned short pppIdleTime __PACK__;
	unsigned char pppConnectType __PACK__;

	unsigned char adslConnectionMode __PACK__;
	unsigned char adslEncapMode __PACK__;
	unsigned char adslMode __PACK__;	// 1: ANSI T1.413, 2: G.dmt, 3: multi-mode, 4: ADSL2, 8: AnnexL, 16: ADSL2+
	unsigned char adslOlr __PACK__;	// adsl capability, 0: disable 1: bitswap 3: SRA & bitswap
	unsigned char ripEnabled __PACK__;
	unsigned char ripVer __PACK__;	// rip version. 0: v1, 1: v2, 2: v1 compatibility

	unsigned char atmLoopback __PACK__;
	unsigned char atmMode __PACK__;
	unsigned char atmVcSwitch  __PACK__;
	unsigned char atmMac1[MAC_ADDR_LEN] __PACK__ ;
	unsigned char atmMac2[MAC_ADDR_LEN] __PACK__ ;
	unsigned char atmVcAutoSearch __PACK__;		// the very first pvc auto-search
	
	unsigned char ipfOutAction __PACK__; // 0 - Deny, 1 - Allow
	unsigned char ipfInAction __PACK__; // 0 - Deny, 1 - Allow
	unsigned char macfOutAction __PACK__; // 0 - Deny, 1 - Allow
	unsigned char macfInAction __PACK__; // 0 - Deny, 1 - Allow
	unsigned char portFwEnabled __PACK__;
	unsigned char dmzEnabled __PACK__;
	unsigned char dmzHost[IP_ADDR_LEN] __PACK__; // DMZ host

	unsigned char snmpSysDescr[SNMP_STRING_LEN] __PACK__;
	unsigned char snmpSysContact[SNMP_STRING_LEN] __PACK__;
	unsigned char snmpSysName[SNMP_STRING_LEN] __PACK__;
	unsigned char snmpSysLocation[SNMP_STRING_LEN] __PACK__;
	unsigned char snmpSysObjectID[SNMP_STRING_LEN] __PACK__;
	unsigned char snmpCommunityRO[SNMP_STRING_LEN] __PACK__;
	unsigned char snmpCommunityRW[SNMP_STRING_LEN] __PACK__;
	unsigned char snmpTrapIpAddr[MAC_ADDR_LEN] __PACK__ ; // MAC address of LAN port in used
	// Kao
	unsigned short brctlAgeingTime __PACK__;
	unsigned char brctlStp __PACK__; // Spanning tree protocol flag, 0 - disabled, 1 - enabled
#ifdef CONFIG_RE8305
	unsigned char mpMode __PACK__; // multi-port admin status: port-mapping, vlan or ipqos
#endif
	unsigned char igmpProxy __PACK__; // IGMP proxy flag, 0 - disabled, 1 - enabled
	unsigned char igmpProxyItf __PACK__; // IGMP proxy interface index
	unsigned char ipptItf __PACK__; // IP passthrough interface index
	unsigned int ipptLTime __PACK__; // IP passthrough max lease time in seconds
	unsigned char ipptLanacc __PACK__;	// enable LAN access
	unsigned char spcEnable __PACK__;	// enable single PC mode
	unsigned char spcIPType __PACK__;	// private IP or IP passthrough
	unsigned char aclcapability __PACK__; // ACL capability flag, 0 - disabled, 1 - enabled
	
#ifdef WLAN_SUPPORT
	// WLAN stuffs
	unsigned char ssid[MAX_SSID_LEN]__PACK__ ; // SSID
	unsigned char channel __PACK__ ;// current channel
//	unsigned char elanMacAddr[6] __PACK__ ; // Ethernet Lan MAC address
	unsigned char wlanMacAddr[6] __PACK__ ; // WLAN MAC address
	unsigned char wep __PACK__ ; // WEP flag, 0 - disabled, 1 - 64bits, 2 128 bits
	unsigned char wep64Key1[WEP64_KEY_LEN] __PACK__ ;
	unsigned char wep64Key2[WEP64_KEY_LEN] __PACK__ ;
	unsigned char wep64Key3[WEP64_KEY_LEN] __PACK__ ;
	unsigned char wep64Key4[WEP64_KEY_LEN] __PACK__ ;
	unsigned char wep128Key1[WEP128_KEY_LEN] __PACK__ ;
	unsigned char wep128Key2[WEP128_KEY_LEN] __PACK__ ;
	unsigned char wep128Key3[WEP128_KEY_LEN] __PACK__ ;
	unsigned char wep128Key4[WEP128_KEY_LEN] __PACK__ ;
	unsigned char wepDefaultKey __PACK__ ;
	unsigned char wepKeyType __PACK__ ;
	unsigned short fragThreshold __PACK__ ;
	unsigned short rtsThreshold __PACK__ ;
	unsigned short supportedRates __PACK__ ;
	unsigned short basicRates __PACK__ ;
	unsigned short beaconInterval __PACK__ ;
	unsigned char preambleType __PACK__; // preamble type, 0 - long preamble, 1 - short preamble
	unsigned char authType __PACK__; // authentication type, 0 - open-system, 1 - shared-key, 2 - both
#ifdef WLAN_ACL
	unsigned char acEnabled __PACK__; // enable/disable WLAN access control
#endif

	unsigned char hiddenSSID __PACK__ ;
	unsigned char wlanDisabled __PACK__; // enabled/disabled wlan interface
	unsigned char aliasName[MAX_NAME_LEN] __PACK__; // device logical name
	unsigned char txPower __PACK__; // TxPower 15/30/60 mW. Mason Yu
	unsigned long inactivityTime __PACK__; // wlan client inactivity time
	unsigned char rateAdaptiveEnabled __PACK__; // enable/disable rate adaptive
	unsigned char dtimPeriod __PACK__; // DTIM period
	unsigned char wlanMode __PACK__; // wireless mode - AP, Ethernet bridge 
	unsigned char networkType __PACK__; // adhoc or Infrastructure
#ifdef WLAN_IAPP
	unsigned char iappDisabled __PACK__; // disable IAPP
#endif

#ifdef WLAN_WPA
	unsigned char encrypt __PACK__; // encrypt type, defined as ENCRYPT_t
	unsigned char enableSuppNonWpa __PACK__; // enable/disable nonWPA client support
	unsigned char suppNonWpa __PACK__; // which kind of non-wpa client is supported (wep/1x)
	unsigned char wpaAuth __PACK__; // WPA authentication type (auto or psk)
	unsigned char wpaCipher __PACK__; // WPA unicast cipher suite
	unsigned char wpaPSK[MAX_PSK_LEN+1] __PACK__; // WPA pre-shared key
	unsigned long wpaGroupRekeyTime __PACK__; // group key rekey time in second
	unsigned char rsIpAddr[4] __PACK__; // radius server IP address
	unsigned short rsPort __PACK__; // radius server port number
	unsigned char rsPassword[MAX_RS_PASS_LEN] __PACK__; // radius server password
	unsigned char enable1X __PACK__; // enable/disable 802.1x
	unsigned char wpaPSKFormat __PACK__; // PSK format 0 - passphrase, 1 - hex
	unsigned char accountRsEnabled __PACK__; // enable/disable accounting server
	unsigned char accountRsIpAddr[4] __PACK__; // accounting radius server IP address
	unsigned short accountRsPort __PACK__; // accounting radius server port number
	unsigned char accountRsPassword[MAX_RS_PASS_LEN] __PACK__; // accounting radius server password
	unsigned char accountRsUpdateEnabled __PACK__; // enable/disable accounting server update
	unsigned short accountRsUpdateDelay __PACK__; // account server update delay time in sec
	unsigned char macAuthEnabled __PACK__; // mac authentication enabled/disabled
	unsigned char rsMaxRetry __PACK__; // radius server max try
	unsigned short rsIntervalTime __PACK__; // radius server timeout
	unsigned char accountRsMaxRetry __PACK__; // accounting radius server max try
	unsigned short accountRsIntervalTime __PACK__; // accounting radius server timeout
	unsigned char wpa2Cipher __PACK__; // wpa2 Unicast cipher
#endif

#ifdef WLAN_WDS
	unsigned char wdsEnabled __PACK__; // wds enable/disable
	unsigned char wdsNum __PACK__; // number of wds entry existed
	WDS_T wdsArray[MAX_WDS_NUM] __PACK__; // wds array
	unsigned char wdsWep __PACK__ ; // WEP flag, 0 - disabled, 1 - 64bits, 2 128 bits
	unsigned char wdsWep64Key1[WEP64_KEY_LEN] __PACK__ ;
	unsigned char wdsWep64Key2[WEP64_KEY_LEN] __PACK__ ;
	unsigned char wdsWep64Key3[WEP64_KEY_LEN] __PACK__ ;
	unsigned char wdsWep64Key4[WEP64_KEY_LEN] __PACK__ ;
	unsigned char wdsWep128Key1[WEP128_KEY_LEN] __PACK__ ;
	unsigned char wdsWep128Key2[WEP128_KEY_LEN] __PACK__ ;
	unsigned char wdsWep128Key3[WEP128_KEY_LEN] __PACK__ ;
	unsigned char wdsWep128Key4[WEP128_KEY_LEN] __PACK__ ;
	unsigned char wdsWepDefaultKey __PACK__ ;
	unsigned char wdsWepKeyType __PACK__ ;
#endif

	unsigned char wlanPrivacyChk __PACK__; // enable/disable wlan privacy check
	unsigned char blockRelay __PACK__; // block/un-block the relay between wireless client
	unsigned char maccloneEnabled __PACK__; // enable NAT2.5 MAC Clone
	
#ifdef WLAN_8185AG
	unsigned char wlanBand __PACK__; // wlan band, bit0-11B, bit1-11G, bit2-11A
	unsigned short fixedTxRate __PACK__; // fixed wlan tx rate, used when rate adaptive is disabled
#endif
#endif
	// Added by Mason Yu for write superUser into Current Setting
	unsigned char suserName[MAX_NAME_LEN] __PACK__;     // user name
	unsigned char suserPassword[MAX_NAME_LEN] __PACK__; // user assword
	unsigned char adslTone[MAX_ADSL_TONE] __PACK__;      // ADSL Tone

} MIB_T, *MIB_Tp;

typedef struct hw_config_setting {
	// Supervisor of web server account
	unsigned char superName[MAX_NAME_LEN] __PACK__ ; // supervisor name
	unsigned char superPassword[MAX_NAME_LEN] __PACK__; // supervisor assword
	unsigned char bootMode __PACK__; // 0 - last config, 1 - default config, 2 - upgrade config	
	unsigned char elanMacAddr[MAC_ADDR_LEN] __PACK__ ; // MAC address of ELAN port in used
	unsigned char wlanMacAddr[MAC_ADDR_LEN] __PACK__ ; // MAC address of WLAN port in used
#if WLAN_SUPPORT
#ifdef WLAN_8185AG
	unsigned char txPowerCCK[MAX_CHAN_NUM] __PACK__; // CCK Tx power for each channel
	unsigned char txPowerOFDM[MAX_CHAN_NUM] __PACK__; // OFDM Tx power for each channel
#else	
	unsigned char txPower[MAX_CHAN_NUM] __PACK__; // Tx power for each channel
#endif
	unsigned char regDomain __PACK__; // regulation domain
	unsigned char rfType __PACK__; // RF module type
	unsigned char antDiversity __PACK__; // rx antenna diversity on/off
	unsigned char txAnt __PACK__; // select tx antenna, 0 - A, 1 - B
	unsigned char csThreshold __PACK__;
	unsigned char ccaMode __PACK__;	// 0, 1, 2
	unsigned char phyType __PACK__; // for Philip RF module only (0 - analog, 1 - digital)
	unsigned char ledType __PACK__; // LED type, see LED_TYPE_T for definition
#endif // of WLAN_SUPPORT
	unsigned char	byte_test __PACK__;
	unsigned short word_test __PACK__;
	unsigned int dword_test __PACK__;
	int	int_test1 __PACK__;
	int	int_test2 __PACK__;	
} HW_MIB_T, *HW_MIB_Tp;

typedef struct chain_record_header {
	unsigned short id __PACK__;
	unsigned int len __PACK__;
} MIB_CHAIN_RECORD_HDR_T, *MIB_CHAIN_RECORD_HDR_Tp;

typedef struct chain_entry {
	struct chain_entry	*pNext;
	unsigned char* pValue;
} MIB_CHAIN_ENTRY_T, *MIB_CHAIN_ENTRY_Tp;





/*
 * Flash File System 
 */
#define DEFAULT_SETTING_MIN_LEN		sizeof(MIB_T)
#define DEFAULT_SETTING_MAX_LEN		0x1000
#define CURRENT_SETTING_MIN_LEN		sizeof(MIB_T)
#ifndef E8B_SUPPORT
#define CURRENT_SETTING_MAX_LEN		0x2000
#else
#define CURRENT_SETTING_MAX_LEN		0x20000
#endif
#define HW_SETTING_MIN_LEN		sizeof(HW_MIB_T)
#define HW_SETTING_MAX_LEN		0x1000

#ifdef __mips__
#define FLASH_DEVICE_NAME		("/dev/mtd")
#ifdef CONFIG_SPANSION_16M_FLASH
#define FLASH_BLOCK_SIZE		0x20000		// 128KB block
#ifdef E8B_SUPPORT

/*ql:20080927 START: modify flash layout, enlarge rootfs to 5M*/
#ifdef NEW_FLASH_LAYOUT
#define WEB_PAGE_OFFSET			0x1E0000	//no sense
#define CS_BAKUP_OFFSET			0x80000
#define ROOTFS_BAKUP_OFFSET		0x600000
#ifdef CONFIG_BACKUP_IMG
#define CODE_IMAGE_OFFSET		0x100000
#else
#define CODE_IMAGE_OFFSET		0x80000
#endif
#else // NEW_FLASH_LAYOUT
#define CODE_IMAGE_OFFSET		0x80000
#define WEB_PAGE_OFFSET			0x1E0000	//It seems of no sense here
#define CS_BAKUP_OFFSET			0x400000
#define ROOTFS_BAKUP_OFFSET		0x480000
#endif // NEW_FLASH_LAYOUT
/*ql:20080927 END*/

#define CODE_IMAGE_OFFSET		0x80000
//---
#define DEFAULT_SETTING_OFFSET	0x20000		//default setting is not in flash, so it is invalid
#define HW_SETTING_OFFSET		0x20000
#define CURRENT_SETTING_OFFSET	0x40000
#else // E8B_SUPPORT
#define CODE_IMAGE_OFFSET		0x20000
//---
#define DEFAULT_SETTING_OFFSET		0x4000
#define HW_SETTING_OFFSET		DEFAULT_SETTING_OFFSET + DEFAULT_SETTING_MAX_LEN
#define CURRENT_SETTING_OFFSET		0x6000
#endif // E8B_SUPPORT
#else // CONFIG_SPANSION_16M_FLASH

#ifndef IMAGE_DOUBLE_BACKUP_SUPPORT
#define FLASH_BLOCK_SIZE			0x10000		// 64KB block
#define DEFAULT_SETTING_OFFSET	0x4000
#define HW_SETTING_OFFSET		0x6000
#define CURRENT_SETTING_OFFSET	0x20000
#define CODE_IMAGE_OFFSET		0x10000
#define WEB_PAGE_OFFSET			0x1E0000
#else // IMAGE_DOUBLE_BACKUP_SUPPORT
/* ----- Start: dual image ----- */
#define DEFAULT_SETTING_OFFSET		0x20000
#define HW_SETTING_OFFSET		0x20000
#define CURRENT_SETTING_OFFSET		0x40000
#define CODE_IMAGE_OFFSET		0x100000
/* ----- End: dual image ----- */
#endif // IMAGE_DOUBLE_BACKUP_SUPPORT

#endif // CONFIG_SPANSION_16M_FLASH
#else // __mips__
#define FLASH_DEVICE_NAME		("setting.bin")
#define HW_SETTING_OFFSET		0
#define DEFAULT_SETTING_OFFSET		HW_SETTING_OFFSET + HW_SETTING_MAX_LEN
#define CURRENT_SETTING_OFFSET		DEFAULT_SETTING_OFFSET + DEFAULT_SETTING_MAX_LEN
#define WEB_PAGE_OFFSET			CURRENT_SETTING_OFFSET + CURRENT_SETTING_MAX_LEN
#define CODE_IMAGE_OFFSET		WEB_PAGE_OFFSET + 0x10000
#endif





/*
 * Chain Record MIB struct
 */
typedef struct ipportfilter_entry {
	unsigned char action __PACK__; // 0 - Deny, 1 - Allow
	//unsigned char ipAddr[IP_ADDR_LEN] __PACK__;
	unsigned char srcIp[IP_ADDR_LEN] __PACK__;
	unsigned char dstIp[IP_ADDR_LEN] __PACK__;
	unsigned char smaskbit __PACK__;
	unsigned char dmaskbit __PACK__;
	//unsigned short fromPort __PACK__;
	unsigned short srcPortFrom __PACK__;
	unsigned short dstPortFrom __PACK__;
	//unsigned short toPort __PACK__;
	unsigned short srcPortTo __PACK__;
	unsigned short dstPortTo __PACK__;
	unsigned char dir __PACK__;
	//unsigned char portType __PACK__;
	unsigned char protoType __PACK__;
	//unsigned char comment[COMMENT_LEN] __PACK__;
} MIB_CE_IP_PORT_FILTER_T, *MIB_CE_IP_PORT_FILTER_Tp;
 
typedef struct portfw_entry {
	unsigned char action __PACK__; // 0 - Deny, 1 - Allow
	unsigned char ipAddr[IP_ADDR_LEN] __PACK__;
	unsigned short fromPort __PACK__;
	unsigned short toPort __PACK__;
	unsigned char protoType __PACK__;
	unsigned char comment[COMMENT_LEN] __PACK__;
} MIB_CE_PORT_FW_T, *MIB_CE_PORT_FW_Tp;

typedef struct macfilter_entry {
	unsigned char action __PACK__; // 0 - Deny, 1 - Allow
	//unsigned char macAddr[MAC_ADDR_LEN] __PACK__;
	unsigned char srcMac[MAC_ADDR_LEN] __PACK__;
	unsigned char dstMac[MAC_ADDR_LEN] __PACK__;
	unsigned char comment[COMMENT_LEN] __PACK__;
	// Added by Mason Yu for Incoming MAC filtering
	unsigned char dir __PACK__;
} MIB_CE_MAC_FILTER_T, *MIB_CE_MAC_FILTER_Tp;

typedef struct atmvc_entry {
	unsigned char ifIndex __PACK__;	// high nibble for PPP, low nibble for mpoa
	unsigned char vpi __PACK__;
	unsigned char qos __PACK__;
	unsigned short vci __PACK__;
	unsigned short pcr __PACK__;
	unsigned short scr __PACK__;
	unsigned short mbs __PACK__;
	unsigned int cdvt __PACK__;
	unsigned char encap __PACK__;
	unsigned char napt __PACK__;
	unsigned char cmode __PACK__;
	unsigned char brmode __PACK__;	// 0: transparent bridging, 1: PPPoE bridging
	unsigned char pppUsername[MAX_NAME_LEN] __PACK__;
	unsigned char pppPassword[MAX_NAME_LEN] __PACK__;
	unsigned char pppAuth __PACK__;	// 0:AUTO, 1:PAP, 2:CHAP
	unsigned char pppACName[MAX_NAME_LEN] __PACK__;
	unsigned char pppCtype __PACK__;
	unsigned short pppIdleTime __PACK__;
	unsigned char ipDhcp __PACK__;
	unsigned char rip __PACK__;
	unsigned char ipAddr[IP_ADDR_LEN] __PACK__;
	unsigned char remoteIpAddr[IP_ADDR_LEN] __PACK__;
	unsigned char dgw __PACK__;
	unsigned int mtu __PACK__;
	unsigned char enable __PACK__;
#ifdef CONFIG_RE8305
	// used for VLAN mapping
	unsigned char vlan __PACK__;
	unsigned short vid __PACK__;
	unsigned short vprio __PACK__;	// 802.1p priority bits
	unsigned char vpass __PACK__;	// vlan passthrough
#endif
} MIB_CE_ATM_VC_T, *MIB_CE_ATM_VC_Tp;

typedef struct iproute_entry {
	unsigned char destID[IP_ADDR_LEN] __PACK__;
	unsigned char netMask[IP_ADDR_LEN] __PACK__;
	unsigned char nextHop[IP_ADDR_LEN] __PACK__;
} MIB_CE_IP_ROUTE_T, *MIB_CE_IP_ROUTE_Tp;


typedef struct aclip_entry {
	unsigned char ipAddr[IP_ADDR_LEN] __PACK__;	
} MIB_CE_ACL_IP_T, *MIB_CE_ACL_IP_Tp;


#ifdef WLAN_SUPPORT
#ifdef WLAN_ACL
typedef struct wlac_entry {
	unsigned char macAddr[6] __PACK__;
	unsigned char comment[COMMENT_LEN] __PACK__;
} MIB_CE_WLAN_AC_T, *MIB_CE_WLAN_AC_Tp;
#endif
#endif

#ifdef CONFIG_RE8305
typedef struct swport_entry {
	// used for Ethernet to PVC mapping
	unsigned char pvcItf;
	// used for VLAN config
	unsigned char pvid;
	unsigned char egressTagAction;
	// used for Link Mode setting
	unsigned char linkMode;	// 10/100 half/full
} MIB_CE_SW_PORT_T, *MIB_CE_SW_PORT_Tp;

typedef struct vlan_entry {
	unsigned char member;
	unsigned short tag;
} MIB_CE_VLAN_T, *MIB_CE_VLAN_Tp;

typedef struct ipqos_entry {
	unsigned char sip[IP_ADDR_LEN] __PACK__;
	unsigned char smaskbit __PACK__;
	unsigned short sPort __PACK__;
	unsigned char dip[IP_ADDR_LEN] __PACK__;
	unsigned char dmaskbit __PACK__;
	unsigned short dPort __PACK__;
	unsigned char protoType __PACK__;
	unsigned char phyPort __PACK__;
	unsigned char outif __PACK__;	// outbound interface
	unsigned char prior __PACK__;	// assign to priority queue
	unsigned char m_ipprio __PACK__;	// mark IP precedence
	unsigned char m_iptos __PACK__;	// mark IP Type of Service
	unsigned char m_1p __PACK__;	// mark 802.1p: 0: none, 1: prio 0, 2: prio 1, ...
} MIB_CE_IP_QOS_T, *MIB_CE_IP_QOS_Tp;
 
#endif

typedef struct acc_entry {
	unsigned char telnet __PACK__;
	unsigned char ftp __PACK__;
	unsigned char tftp __PACK__;
	unsigned char web __PACK__;
	unsigned char snmp __PACK__;
	unsigned char ssh __PACK__;
	unsigned char icmp __PACK__;
} MIB_CE_ACC_T, *MIB_CE_ACC_Tp;

extern BOOT_TYPE_T __boot_mode;




/* ------------------------------------------------------------
 * MIB API
 * ------------------------------------------------------------ */
int mib_update_from_raw(unsigned char* ptr, int len); /* Write the specified setting to flash, this function will also check the length and checksum */
int mib_read_to_raw(CONFIG_DATA_T data_type, unsigned char* ptr, int len); /* Load flash setting to the specified pointer */
int mib_update(CONFIG_DATA_T data_type); /* Update RAM setting to flash */
int mib_read_header(CONFIG_DATA_T data_type, PARAM_HEADER_Tp pHeader); /* Load flash header */
int mib_load(CONFIG_DATA_T data_type); /* Load flash setting to RAM */
int mib_reset(CONFIG_DATA_T data_type); /* Reset to default */
int mib_update_firmware(unsigned char* ptr, int len); /* Update Firmware */

int mib_init(void); /* Initialize */
int mib_get(int id, void *value); /* get mib value */
int mib_set(int id, void *value); /* set mib value */
#ifdef INCLUDE_DEFAULT_VALUE
int mib_init_mib_with_program_default(CONFIG_DATA_T data_type);
#endif

unsigned int mib_chain_total(int id); /* get chain record size */
void mib_chain_clear(int id); /* clear chain record */
int mib_chain_add(int id, unsigned char* ptr); /* add chain record */
int mib_chain_delete(int id, unsigned int recordNum); /* delete the specified chain record */
unsigned char* mib_chain_get(int id, unsigned int recordNum); /* get the specified chain record */

#endif // INCLUDE_MIB_H
