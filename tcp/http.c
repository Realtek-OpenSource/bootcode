
//#include "../gzip/gzip.h"
#ifdef	ERROR
#undef	ERROR
#endif
//#include <stdio.h>
#include "../tftpnaive/net.h"
#include "../inc/board.h"
//#include "../../inc/flashdrv.h"
//#include "../../inc/rtl_image.h"
#include "memp.h"
#include "http.h"


//extern board_param_t bdinfo;

#define		IMAGE_FILE		1
#define		BOOTLOADER_FILE	2
#define		LOAD_IMAGE		3

//#define		DRAM_MAP_LOADER_ADDR		0x81c00000

//#define		PBUF_MAX_LEN		TCP_MSS+80+80+18
#define		HTTP_MAX_SIZE		1024
//static	unsigned int  http_ram_pool[HTTP_MAX_SIZE>>2]={0};
/*
static	unsigned char indexdata[]="HTTP/1.0 200 OK\r\n\
Content-type: text/html\r\n\
\r\n\
<html>\
<head><title>A test page</title></head>\
<body>\
This is a samll test page.\
<form method=POST>\
Send this file: <input name=userfile type=file>\
<input type=submit value=Send File>\
</form>\
</body> \
</html>";
*/
/*
static unsigned char setfile[]="HTTP/1.0 200 OK\r\n\
Content-type: text/html\r\n\
\r\n\
<html>\
<head><title>Set File Type</title></head>\
<body>\
<form action=/cgi-bin/post-query method=POST>\
<input type=radio name=f value=i>\
        imagefile<p>\
<input type=radio name=f value=l>\
        bootloader<p>\
<input type=radio name=f value=r>\
        load image without updating Flash<p>\
<input type=submit><input type=reset>\
</form>\
</body> \
</html>";
*/


#ifdef WEB_ACCOUNT_SUPPORT
static unsigned char autodata[]="HTTP/1.0 200 OK\r\n\
Content-type: text/html\r\n\
\r\n\
<html>\
<head>\
 <meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\
<script language=\"javascript\">\
function goback() { \
	eval(\"location=' /upload.htm' \" ); \
}\
setTimeout(\"goback()\", 0); \
</script>\
</head>\
</html>";

static unsigned char logindata[]="HTTP/1.0 200 OK\r\n\
Content-type: text/html\r\n\
\r\n\
<html>\
<head>\
 <meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\
<title>网关登陆</title>\
</head>\
<script language=\"javascript\">\
function onlogin() {\
	with ( document.forms[0] ) {\
		if(username.value.length <= 0) {\
			alert(\"用户名为空，请输入用户名!\");\
			return;\
		}\
		var loc =\"login.cgi?username=\" + username.value +\"&psd=\" + password.value;\
		var code = 'location=\"' + loc + '\"';\
		eval(code);\
	}\
}\
</script>\
<body leftmargin=\"0\" topmargin=\"0\"  bgcolor=\"white\" onload=\"document.forms[0].username.focus()\">\
<form>\
<table width=\"100%\" height=\"100%\" align=\"center\" valign=\"middle\" >\
<tr>\
	<td width=\"25%\" height=\"25%\"></td>\
	<td width=\"50%\" ></td>\
	<td width=\"25%\"></td>\
</tr>\
<tr  align=\"left\" valign=\"top\">\
	<td height=\"50%\"></td>\
	<td>\
		<table width=\"100%\" cellspacing=\"0\"  style=\"font-size:10pt\">\
			<tr bgcolor=\"#427594\" height=\"10%\">\
				<td  bgcolor=\"#427594\" width=\"1\"></td>\
				<td>&nbsp;&nbsp;<font color=\"white\">升级登陆\
				</font></td>\
				<td  bgcolor=\"#427594\" width=\"1\"></td>\
			</tr>\
			<tr  align=\"center\" valign=\"middle\" >\
				<td  bgcolor=\"#427594\" width=\"1\"></td>\
				<td><table  cellspacing=\"8\"  style=\"font-size:10pt\">\
					<tr><td width=\"21\">&nbsp;&nbsp;&nbsp;</td><td>用户:</td><td><input type=\"text\" name=\"username\" maxlength=\"30\" id=\"username\" style=\"width:150;\"/></td></tr>\
					<tr><td width=\"21\">&nbsp;&nbsp;&nbsp;</td><td>密码:</td><td><input type=\"password\" name=\"password\" maxlength=\"30\"  id=\"password\" style=\"width:150;\"/></td></tr>\
				</table></td>\
				<td  bgcolor=\"#427594\" width=\"1\"></td>\
			</tr>\
			<tr bgcolor=\"#427594\" height=\"10%\"  align=\"center\" valign=\"middle\" >\
				<td  bgcolor=\"#427594\" width=\"1\"></td>\
				<td>&nbsp;&nbsp;<input type=\"button\" onclick=\"onlogin()\" value=\"登录\" />\
				&nbsp;&nbsp;<input type=\"reset\" value=\"重写\" /></td>\
				<td  bgcolor=\"#427594\" width=\"1\"></td>\
			</tr>\
		</table>\
	</td>\
	<td></td>\
</tr>\
<tr>\
	<td height=\"25%\"></td>\
	<td></td>\
	<td></td>\
</tr>\
</table>\
</form>\
</body>\
</html>";
#define ACCOUNTLEN 30
static unsigned char username[ACCOUNTLEN+1]="telecomadmin";
static unsigned char password[ACCOUNTLEN+1]="nE7jA%5m";		
static int account_authenticated=0;

int updateAccount(void)
{
	//update Account info from e8b mib to username and password
	printf("update account info from mib, to be implemented.\n");
	printf("\n");
	return 1;
}

// info format:    "username=xxxx&psd=yyyy "
static int AccountAutheticateOK(unsigned char * info)
{
	unsigned char *strptr;
	unsigned char localusername[ACCOUNTLEN+1]={0};
	unsigned char localpassword[ACCOUNTLEN+1]={0};	
	strptr=info;
	if(!memcmp(strptr, "username=", sizeof("username=")-1))
	{
		unsigned char *localstrptr;
		
		int i=0;
		strptr+=sizeof("username=")-1;
		localstrptr=strptr;
		while(*localstrptr != '&'  && i<(1+ACCOUNTLEN))
		{
			i++;
			localstrptr++;
		}
		if(i < (1+ACCOUNTLEN))
		{
			//found the boundary of username's xxxx
			memcpy(localusername, strptr, localstrptr-strptr);
			strptr=localstrptr+1; //point to string "psd=yyyy"
			if(!memcmp(strptr, "psd=", sizeof("psd=")-1))
			{
				//psd is received...
				strptr+=sizeof("psd=")-1;
				//point to yyyy
				localstrptr=strptr;
				i=0;
				while(*localstrptr != ' '  && i<(1+ACCOUNTLEN))
				{
					i++;
					localstrptr++;
				}
				if(i < (1+ACCOUNTLEN))
				{
					//found the boundary of password's yyyy
					memcpy(localpassword, strptr, localstrptr-strptr);
					//printf("%s: %d: localusername=%s, localpass=%s,\n", __FUNCTION__, __LINE__, localusername, localpassword);
					//printf("%s: %d: username=%s, pass=%s,\n", __FUNCTION__, __LINE__, username, password);
					if(!strcmp(localusername, username)  && !strcmp(localpassword, password))
					{
						//autheticated...
						account_authenticated=1;
						return 1;
					}
				}
			}
		}
	}
	//authentication failed
	account_authenticated=0;
	return 0;
}

#endif
#ifdef E8B_SUPPORT

static	unsigned char indexdata[]="HTTP/1.0 200 OK\r\n\
Content-type: text/html\r\n\
\r\n\
<html>\
   <head>\
      <meta HTTP-EQUIV='Pragma' CONTENT='no-cache'>\
            <script language=\"javascript\">\
var progress = 0;\
function isInProgress() {\
   if ( progress == 0 )  {\
      progress = 1;\
      return true;\
   }\
   alert('升级软件正在进行中.请等待一分钟.');\
   return false;\
}\
</script>\
   </head>\
   <body>\
      <blockquote>\
         <form method='post' ENCTYPE='multipart/form-data' action='upload.cgi' onSubmit='return isInProgress();'>\
            <br>\
    <b>步骤 1:</b> 从您的ISP获得一个最新的image软件。<br>\
            <br>\
    <b>步骤 2:</b> 在下面的选择框中输入image文件的路径或点击&quot;浏览&quot;按钮寻找image文件。<br>\
            <br>\
    <b>步骤 3:</b> 点击\"升级软件\"按钮来升级新的image文件。<br>\
            <br>\
    注意: 升级过程大概持续2分钟.<br>\
            <br>\
            <table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\
               <tr>\
                  <td>软件的文件名:&nbsp;\
                  </td>\
                  <td><input type='file' name='filename' size='15'></td>\
               </tr>\
            </table>\
            <p align=\"center\"><input type='submit' value='升级软件'></p>\
         </form>\
      </blockquote>\
   </body>\
</html>";

static	unsigned char postresponse[]="HTTP/1.0 200 OK\r\n\
Content-type: text/html\r\n\
\r\n\
<html>\
<head><META HTTP-EQUIV=Refresh CONTENT=50>\
<title>软件升级</title></head>\
<body>\
软件升级中. 请等待...\
</body> \
</html>";
//static	unsigned char postresponse[]="HTTP/1.0 200 OK\r\n";

static	unsigned char updateerror[]="HTTP/1.0 200 OK\r\n\
Content-type: text/html\r\n\
\r\n\
<html>\
<head><title>软件升级</title></head>\
<body>\
升级失败! 请刷新网页重新升级.\
</body> \
</html>";

static	unsigned char updateok[]="HTTP/1.0 200 OK\r\n\
Content-type: text/html\r\n\
\r\n\
<html>\
<head><title>软件升级</title></head>\
<body>\
升级成功!请重启系统.\
</body> \
</html>";
#else
static	unsigned char indexdata[]="HTTP/1.0 200 OK\r\n\
Content-type: text/html\r\n\
\r\n\
<html>\
<head><title>Update Firmware</title></head>\
<body>\
<form enctype=multipart/form-data method=post>\
Upload image file: <input name=userfile type=file>\
<input type=submit value=Send File>\
</form>\
</body> \
</html>";

static	unsigned char postresponse[]="HTTP/1.0 200 OK\r\n\
Content-type: text/html\r\n\
\r\n\
<html>\
<head><META HTTP-EQUIV=Refresh CONTENT=40>\
<title>Update Firmware</title></head>\
<body>\
Start upgrade firmware. Please wait...\
</body> \
</html>";
//static	unsigned char postresponse[]="HTTP/1.0 200 OK\r\n";

static	unsigned char updateerror[]="HTTP/1.0 200 OK\r\n\
Content-type: text/html\r\n\
\r\n\
<html>\
<head><title>Update Firmware</title></head>\
<body>\
Upgrade failed! Please refresh page and upgrade again.\
</body> \
</html>";

static	unsigned char updateok[]="HTTP/1.0 200 OK\r\n\
Content-type: text/html\r\n\
\r\n\
<html>\
<head><title>Update Firmware</title></head>\
<body>\
Upgrade complete. Please restart system.\
</body> \
</html>";
#endif


//unsigned char	boundary[85];

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

//static	unsigned char	http_ram[HTTP_MAX_SIZE] = 0;
//static	unsigned char	*const http_ram = (unsigned char *)DRAM_MAP_DLOAD_BUF_ADDR+0x3e0000-HTTP_MAX_SIZE;
//static	unsigned char	*const http_ram = (unsigned char *)DRAM_MAP_RUN_IMAGE_ADDR;
//static	unsigned char	*const http_ram = (unsigned char *)DRAM_MAP_DLOAD_BUF_ADDR;
static	unsigned char	*const http_ram = (unsigned char *)(((unsigned char*)MEM_ALIGN(DRAM_MAP_DLOAD_BUF_ADDR))+
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
															sizeof(struct mem))+
															MEM_ALIGN_SIZE(sizeof(struct IPData))
															);
//static	unsigned short	http_ram_head = 0;
static	unsigned short	http_ram_tail = 0;


u8_t			has_set_file_type = 0;
u8_t			is_linux = 0;
static u16_t	boundary_len = 0;
static s16_t	head_len = -1;
static int		content_length = 0;
static int		content_recv_len = 0;
static int		UpdateFWOK=0;

static void		http_write_loader(struct tcp_pcb *pcb, u8_t *ptr, int len);


err_t	http_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	int		ip;

//	printf("http_ram is 0x%x.\n", http_ram);
	ip = newpcb->remote_ip;
	newpcb->recv =  http_recv;
//	http_post_flag = 0;
//	content_length = 0;
//	tcp_recv(newpcb, http_recv);
	printf("*************************************\nConnect is established.\nFrom IP: %d.%d.%d.%d.\n*************************************\n", ((ip>>24)&0xff),((ip>>16)&0xff),((ip>>8)&0xff),(ip&0xff));
//	printf("*************************************\n");
//	printf("Connect is established.\n");
//	printf("From IP: %d.%d.%d.%d.\n", ((ip>>24)&0xff),((ip>>16)&0xff),((ip>>8)&0xff),(ip&0xff));
//	printf("*************************************\n");
	return ERR_OK;
}

err_t 	http_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	struct pbuf		*next;
	int			i;
//	unsigned char	*ptr;

	if (p->tot_len==1||p->tot_len==0) {
		return ERR_OK;
	}

	for(next=p;next!=NULL;next=next->next) {
		bcopy(next->payload, &http_ram[http_ram_tail], next->len);
		http_ram_tail += next->len;
	}

	pbuf_free(p);
	if (content_length==0) {
//		int			j;
		if (http_ram[0] == 'G'&&http_ram[1] == 'E'&&http_ram[2] == 'T') {
//			tcp_write(pcb, setfile, sizeof(setfile), 1);
#ifdef WEB_ACCOUNT_SUPPORT
			if(account_authenticated){
#endif				
			switch (UpdateFWOK) {
				case -1:
			        tcp_write(pcb, updateerror, sizeof(updateerror), 1);
			        UpdateFWOK=0; //for user to upgrade again
			        break;
				case 1:
			        tcp_write(pcb, updateok, sizeof(updateok), 1);
			        break;
				default:
			        tcp_write(pcb, indexdata, sizeof(indexdata), 1);
			        break;
			};
#ifdef WEB_ACCOUNT_SUPPORT			
				}else{
				//try get the authentication infos from http GET entity
					if(!memcmp(&(http_ram[4]), "/login.cgi", 10))
					{
						if(AccountAutheticateOK(&(http_ram[4+sizeof("/login.cgi")-1+1])))
						{
							tcp_write(pcb, autodata, sizeof(autodata), 1);
						}
						else
						{
							tcp_write(pcb, logindata, sizeof(logindata), 1);
						}
					}else
					{
						// transmit login web to web browser to authentication...
						tcp_write(pcb, logindata, sizeof(logindata), 1);
					}
				}
#endif			
//		} else if (http_ram[0] == 'H'&&http_ram[1] == 'E'&&http_ram[2] == 'A'&&http_ram[3] == 'D') {
//			tcp_write(pcb, &indexdata[9], 9, 1);
		} else if ((http_ram[0] == 'P'&&http_ram[1] == 'O'&&http_ram[2] == 'S'&&http_ram[3] == 'T')||(is_linux==1)) {
//			printf("0. Post here\n");
				for(i=0;i<http_ram_tail;i++) {
					if (http_ram[i]=='b'&&http_ram[i+1]=='o'&&http_ram[i+2]=='u'&&http_ram[i+3]=='n'&&
						http_ram[i+4]=='d'&&http_ram[i+5]=='a'&&http_ram[i+6]=='r'&&http_ram[i+7]=='y'
						&&http_ram[i+8]=='=') {
						i += 9;
	//					printf("\n");
						while(i<http_ram_tail) {
							if (http_ram[i]=='\r'&&http_ram[i+1]=='\n') {
								break;
							}
//							printf("%c",http_ram[i]);
							i++;
							boundary_len++;
						}
					}
				}

				if (boundary_len==0) {	// in linux
					http_ram_tail = 0;
					tcp_recved(pcb,p->tot_len);
					head_len = 0;
					is_linux = 1;
//					printf("Is linux.\n");
					return ERR_OK;
				}
				// find out the content-length
				for(i=0;i<http_ram_tail;i++) {
					while(1) {
						if (http_ram[i]=='\r'&&http_ram[i+1]=='\n') {
							i+=2;
							break;
						}
						i++;
					}
					
					if (http_ram[i]=='C'&&http_ram[i+1]=='o'&&http_ram[i+2]=='n'&&http_ram[i+3]=='t'&&
						http_ram[i+4]=='e'&&http_ram[i+5]=='n'&&http_ram[i+6]=='t'&&http_ram[i+7]=='-'&&
						http_ram[i+8]=='L'&&http_ram[i+9]=='e'&&http_ram[i+10]=='n'&&http_ram[i+11]=='g') {
						i += 15;
						content_length = atoi(&http_ram[i]);
						break;
					}
				}

//				printf("content_length is %d.\n",content_length);
				if (is_linux==1) {
					while(i<http_ram_tail) {
						if (http_ram[i]=='\r'&&http_ram[i+1]=='\n'&&http_ram[i+2]=='\r'&&http_ram[i+3]=='\n') {
							i+=4;
							break;
						}
						i++;
					}
					content_recv_len = i;
//					printf("2. content_recv_len is: %d.\n", content_recv_len);
					while(i<http_ram_tail) {
						if (http_ram[i]=='\r'&&http_ram[i+1]=='\n'&&http_ram[i+2]=='\r'&&http_ram[i+3]=='\n') {
							i+=4;
//							content_length -= 4;
							head_len = i;
							break;
						}
						i++;
//						content_length--;
					}
					content_length -= head_len-content_recv_len;
					bcopy(&http_ram[head_len], &http_ram[TCP_WND+MEM_ALIGN_SIZE(head_len)], http_ram_tail-head_len);
					content_recv_len = (http_ram_tail-head_len);
//					content_recv_len = http_ram_tail - content_recv_len;
//					printf("2. content_recv_len is: %d.\n", content_recv_len);
				}
/*star:20091221 START sometimes the first packet include the http head and http data*/
				else{
					while(i<http_ram_tail) {
						if (http_ram[i]=='\r'&&http_ram[i+1]=='\n'&&http_ram[i+2]=='\r'&&http_ram[i+3]=='\n') {
							i+=4;
							break;
						}
						i++;
					}
					content_recv_len = i;
	//				printf("2. content_recv_len is: %d.\n", content_recv_len);
					while(i<http_ram_tail) {
						if (http_ram[i]=='\r'&&http_ram[i+1]=='\n'&&http_ram[i+2]=='\r'&&http_ram[i+3]=='\n') {
							i+=4;
							head_len = i;
							break;
						}
						i++;
					}
					if(head_len>-1){
						content_length -= (head_len-content_recv_len);
						bcopy(&http_ram[head_len], &http_ram[TCP_WND+MEM_ALIGN_SIZE(head_len)], http_ram_tail-head_len);
						content_recv_len = (http_ram_tail-head_len);
	//					content_recv_len = http_ram_tail - content_recv_len;
	//					printf("2. content_recv_len is: %d.\n", content_recv_len);
					}
				}
/*star:20091221 END*/
//				printf("content_length is %d.\n",content_length);
//				while(i<http_ram_tail) {
//					if (http_ram[i]=='\r'&&http_ram[i+1]=='\n'&&http_ram[i+2]=='\r'&&http_ram[i+3]=='\n') {
//						i+=4;
//						break;
//					}
//					i++;
//				}
	//			printf("http_ram_tail is %d, i is %d.\n",http_ram_tail,i);
	//			printf("1. content_recv_len is %d.\nData is: \n",content_recv_len);
	//			content_recv_len = (http_ram_tail-i);
	//			while(i<http_ram_tail) {
	//				printf("%c", http_ram[i++]);
	//			}
	//			bcopy(&http_ram[i], &http_ram[TCP_WND], http_ram_tail-i);
	//			content_recv_len = (http_ram_tail-i);
//				printf("2. content_recv_len is: %d.\n", content_recv_len);

				if (content_length>content_recv_len) {
					http_ram_tail = 0;
					tcp_recved(pcb,p->tot_len);
					printf("Waiting for uploading......\n\r");
					return ERR_OK;;
				}
				content_recv_len = content_length = 0;
			}
//		}
		http_ram_tail = 0;
		tcp_close(pcb);
	} else {
		if (head_len==-1) {
//			printf("Here 0\n");
			for (i=0;i<http_ram_tail;i++) {
				if (http_ram[i]=='\r'&&http_ram[i+1]=='\n'&&http_ram[i+2]=='\r'&&http_ram[i+3]=='\n') {
					head_len = (i+4);
					break;
				}
			}
//			printf("align len is %d, len is %d, address is 0x%x.\n", MEM_ALIGN_SIZE(head_len), head_len, &http_ram[TCP_WND]);
			bcopy(&http_ram[head_len], &http_ram[TCP_WND+MEM_ALIGN_SIZE(head_len)], http_ram_tail-head_len);
			content_length -= head_len;
			content_recv_len = (http_ram_tail-head_len);
		} else {
//			printf("Here 1\n");
			bcopy(http_ram, &http_ram[TCP_WND+MEM_ALIGN_SIZE(head_len)+content_recv_len], http_ram_tail);
			content_recv_len += http_ram_tail;
		}
		
		http_ram_tail = 0;
//		printf("2. content_recv_len is: %d.\n", content_recv_len);
		if (content_length<=content_recv_len) {
//			printf("\nDownload over.\n");
			tcp_write(pcb, postresponse, sizeof(postresponse), 1);
			tcp_close(pcb);
			http_write_loader(pcb, &http_ram[TCP_WND+MEM_ALIGN_SIZE(head_len)],content_length);
			content_recv_len = content_length = head_len = 0;
		}
		tcp_recved(pcb,p->tot_len);
		tcp_output(pcb);
	}
}


extern board_param_t bParam;
static void		http_write_loader(struct tcp_pcb *pcb, u8_t *ptr, int len)
{
	char *app_buf;

	//	octet-stream....
//	if (len>TCP_MSS) 
		len -= (boundary_len+8);

	/* update run image */
	/*
	printf("\nData start is :\n");
	int 	i;
	for(i=0;i<500;i++) {
		printf("%02x",*(ptr+i));
	}
	printf("\n\n");

	printf("\nData end is :\n");
	for(i=len-500;i<len;i++) {
		printf("%02x",*(ptr+i));
	}
	printf("\n\n");
	*/
	get_param(&bParam);	
	app_buf = (char*)bParam.app;
	if (writeImage(ptr)) {
#ifdef E8B_SUPPORT
		// e8b boot do not support arbitary binary file update.....
		printf("illegal image file\n\r");
		UpdateFWOK= -1;
#else
		printf("No header, assuming raw binary\n\r");
		if ( write_file(app_buf, ptr, len) != 0 ) {
			printf("\nUpdate runtime image error.\n");
			UpdateFWOK=-1;
		} else {
			UpdateFWOK=1;
		};
#endif		
	} else {
		UpdateFWOK=1;
	}

#if 0
restart:
	char	ch;
	void (*appStart)(void);
	printf("\n\nRESTART ...");
//	getchar(&ch);
	//tick_Delay100ms(5);
	delay_msec(500);
	appStart = (void*)CPU_REBOOT_ADDR;
	appStart();
#endif	
}

