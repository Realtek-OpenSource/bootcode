/*
	X-Modem Header File

	1999/09/03	sprite, support Xmode Tx & Rx
*/

#ifndef	_XMODE_H_
#define	_XMODE_H_

/*****************
 * X-Modem status
 *****************/
#define	XMODEM_OK		1
#define	XMODEM_CANCEL	2
#define	XMODEM_ACK		3
#define	XMODEM_NAK		4
#define	XMODEM_COMPLETE	5
#define	XMODEM_NO_SESSION	6
#define	XMODEM_ABORT	7
#define	XMODEM_TIMEOUT	8

/****************************
 * flow control character
 ****************************/
#define	SOH	0x01		/* Start of header */
#define	EOT	0x04		/* End of transmission */
#define	ACK	0x06		/* Acknowledge */
#define	NAK	0x15		/* Not acknowledge */
#define	CAN	0x18		/* Cancel */
#define	ESC	0x1b		/* User Break */

/****************************
 * Xmode paramters
 ****************************/
#define	FRAME_SIZE	132	/* X-modem structure */
#define	BUFFER_SIZE	128	/* X-modem buffer */
#define	TIMEOUT		180	/* max timeout */
#define	RETRY_COUNT	20	/* Try times */
#define	xWAITTIME	0x00400000	/* waitiing time */

/***********************
 * frame structure
 ***********************/
typedef	struct
{
	 unsigned char	soh;
	 unsigned char	recordNo;
	 unsigned char	recordNoInverted;
	 unsigned char	buffer[BUFFER_SIZE];
	 unsigned char	CRC;
} XMODEM_FRAME;

/******************************
 * some MACRO for var Driver
 ******************************/

#define	COMMPORT_POLL() \
	poll_cc()

#define	COMMPORT_GET(P,c) \
	c = getcc()

#define	COMMPORT_PUT(P,c) \
	put_cc(c)

#define	COMMPORT_GET_T(P,c,t) \
{ \
	while (t != 0) \
	{ \
		if (COMMPORT_POLL()) \
		{ \
			COMMPORT_GET(P,c); \
			break; \
		} \
		t--; \
	} \
}

extern	short	xModemStart(void);
extern	short	xModemEnd(void);
extern	short	xModemInquiry(unsigned char code);
extern	short	xModemCancel(void);
extern	void	xModemGetFirst(void);
extern	void	xModemGetOthers(unsigned char *ptr);
extern	short	xModemRxBlock(unsigned char ptr[]);
extern	short	xModemTxBlock(unsigned char *ptr, short count);
extern	short	xModemEOT(void);
extern	short	xModemRxFrame(unsigned char *buffer);
extern	short	xModemTxFrame(unsigned char *buffer);
extern	int	xModemRxBuffer(unsigned char *buffer, int MaxSize);
extern	short	xModemRxFile(short Handle);
extern	int	xModemTxFile(short Handle);
extern	void	xModemDelay(void);

#endif /* _XMODE_H_ */

