/*
*/
#include "xmodem.h"

#define TRUE	1
#define FALSE	0


int	Using = FALSE;
short	currentFrame;	/* current frame number */
short	previousFrame;	/* previous frame number */
short	expected;
short	retry;
short	rStatus;
int	rFinish = FALSE;
short	Port = 0;
XMODEM_FRAME	Xframe;


short	xModemStart(void)
{
	/* check PHY Driver */

	/* check X-Modem status */
	if (Using == TRUE)
		return XMODEM_NO_SESSION;

	/* initial COM port & buffer */

	/* mark X-Modem startup */
	Using = TRUE;

	return XMODEM_OK;
}

short	xModemEnd(void)
{
	/* check X-Modem status */
	if (Using == FALSE)
		return XMODEM_NO_SESSION;

	
	/* Stop COM port */
	
	/* mark X-Modem Stop */
	Using = FALSE;

	return XMODEM_OK;
}

void	xModemDelay()
{
 unsigned int	d;

	for (d=0;d<0xffff;d++)
	/* NULL */;
}

short	xModemInquiry(unsigned char code)
{
 int	c;

	if (code != ACK)
		xModemDelay();

	if (code == NAK)
		c = 1;
	if (code == CAN)
		return TRUE;	/* MUST clear buffer */

	COMMPORT_PUT(Port,code);
	return TRUE;
}

short	xModemCancel(void)
{
	if (Using == FALSE)
		return (XMODEM_NO_SESSION);

	if (xModemInquiry(CAN) == -1)
		return	XMODEM_NAK;
	else
		return XMODEM_OK;
}

void	xModemGetFirst(void)
{
 char		ch = 0;
 unsigned int	cTimeout;

	cTimeout = xWAITTIME;
	COMMPORT_GET_T(Port,ch,cTimeout);	
	if (cTimeout == 0)
	{
		rFinish = TRUE;
		rStatus =  XMODEM_TIMEOUT;
		return ;
	}

	ch = ch & 0xff;
	switch(ch)
	{
		case CAN :
					rFinish = TRUE;
					rStatus = XMODEM_CANCEL;
					break;
		case EOT :
					rFinish = TRUE;
					xModemInquiry(ACK);
					rStatus = XMODEM_COMPLETE;
					break;
		case SOH :
					expected =  FRAME_SIZE - 1;
					break;
		case ESC :
					rFinish = TRUE;
					rStatus = XMODEM_ABORT;
					break;
		default :
					xModemInquiry(NAK);
					break;
	}
	
}

void	xModemGetOthers(unsigned char *ptr)
{
 unsigned char		uch = 0;
 unsigned char		uchk;
 short	summation;
 unsigned int	cTimeout;
 short	j;

	cTimeout = xWAITTIME;
	COMMPORT_GET_T(Port,uch,cTimeout);
	if (cTimeout == 0)
	{
		rFinish = TRUE;
		rStatus = XMODEM_TIMEOUT;
		return ;
	}

	if (uch == previousFrame)
	{
		retry = 0;
		xModemInquiry(ACK);
	}
	else
	{
		rStatus = XMODEM_OK;
	
		/* check record number */
		if (uch != currentFrame)
			rStatus = XMODEM_NAK;

		/* check invert number */
		cTimeout = xWAITTIME;
		COMMPORT_GET_T(Port,uch,cTimeout);
		if (cTimeout == 0)
		{
			rFinish = TRUE;
			rStatus = XMODEM_TIMEOUT;
			return ;
		}

		uchk = ((~currentFrame) & 0xff);
		if (uch != uchk)
			rStatus = XMODEM_NAK;

		/* get data */
		summation = 0;
		for (j=0; j<BUFFER_SIZE; j++)
		{
			cTimeout = xWAITTIME;
			COMMPORT_GET_T(Port,uch,cTimeout);
			if (cTimeout == 0)
			{
				rFinish = TRUE;
				rStatus = XMODEM_TIMEOUT;
				return ;
			}
			*(ptr+j) = uch;
			summation += *(ptr+j);
		}
	
		/* CRC check */
		cTimeout = xWAITTIME;
		COMMPORT_GET_T(Port,uch,cTimeout);
		if (cTimeout == 0)
		{
			rFinish = TRUE;
			rStatus = XMODEM_TIMEOUT;
			return ;
		}

		uchk = summation & 0xff;
		if (uchk != uch)
			rStatus = XMODEM_NAK;
		

		if (rStatus == XMODEM_OK)
		{
			rFinish = TRUE;
			xModemInquiry(ACK);
		}
		else
		{
			xModemInquiry(NAK);
		}
		
	}
}

short	xModemRxBlock(unsigned char ptr[])
{

	expected = 1;
	rFinish = FALSE;
	while (rFinish == FALSE)
	{
		if (expected == 1)
		{
			xModemGetFirst();
		}
		else
		{
			xModemGetOthers(ptr);
		}
	}
	return rStatus;
}

short	xModemTxBlock(unsigned char *ptr, short count)
{
 short	i,_count;

	for (i=0;i<RETRY_COUNT;i++)
	{
		_count = count;
		while (_count > 0)
		{
			COMMPORT_PUT(Port,*ptr);
			_count--;
			ptr++;
		}
		
	}
	return TRUE;
 
}

short	xModemEOT(void)
{
 unsigned char	buffer;

	buffer = EOT;
	
	/* clean Input & Output Port */

	return (xModemTxBlock(&buffer,1));
}


short	xModemRxFrame(unsigned char *buffer)
{
 short	status;

	status = xModemRxBlock(buffer);
	if (status == XMODEM_OK)
	{
		previousFrame = currentFrame;
		currentFrame = (currentFrame+1) & 0xff;
	}

	return status;
}

//#define _HAVE_TIMEOUT

int	xModemRxBuffer(unsigned char *buffer, int MaxSize)
{
 short	retry;
 short	status;
 int	rxlen;
 int	_timeout;
 int	finished,tooLarge;


RxB_again:
	/* MUST clear PHY buffer .... */
	currentFrame = 1;
	previousFrame = 0;	/* previous frame number */
	tooLarge = FALSE;
	rxlen = 0;
	/* send NAK to client */
	retry = 0;

#ifdef	_HAVE_TIMEOUT

	while (retry < RETRY_COUNT)

#else

	while (1)

#endif
	{

		xModemInquiry(NAK);

		/* delay time */
		_timeout =  xWAITTIME;
		while (_timeout != 0)
		{
			_timeout--;
			if (COMMPORT_POLL())
				break;
		}

		if (COMMPORT_POLL())
		{
				break;
		}
	
		retry++;
	}

#ifdef	_HAVE_TIMEOUT

	/* TimeOut or not */
	if (retry == RETRY_COUNT)
	{
		return 0;
	}

#endif

	finished = FALSE;
	while (finished == FALSE)
	{
		status = xModemRxFrame(buffer+rxlen);
		
		/*
			If rx any non-SOH character
		*/
		if ((status == XMODEM_TIMEOUT) && (rxlen == 0)) {
			goto RxB_again;
		}
	
		switch (status)
		{
			case XMODEM_OK :
						rxlen += BUFFER_SIZE;
						if (rxlen > MaxSize)
						{
							rxlen -= BUFFER_SIZE;
							tooLarge = TRUE;
							finished = TRUE;
							xModemInquiry(NAK);
						}
						break;
			case XMODEM_COMPLETE :
						status = XMODEM_OK;
						finished = TRUE;
						break;
			case XMODEM_ABORT :
						status = XMODEM_ABORT;
						rxlen = 0;
			default :
						xModemCancel();
						finished = TRUE;
						rxlen = 0;
						break;
		}
	} 

	if (tooLarge == TRUE)
	{
		return MaxSize+1;
	}
	else
	{
		return rxlen;
	}
}

