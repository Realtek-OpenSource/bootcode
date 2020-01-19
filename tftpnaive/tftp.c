
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

void processTFTP (TFTPDpkt * tp)
{
	register int nbtry;
	TFTPDpkt tftpd_pkt;

	if (!tftp_req)
		return;

	if (tftp_req->port == 0)
		tftp_req->port = tp->udphdr.src_port;

	if (tp->udphdr.iphdr.src_ip == tftp_req->server
		&& tp->udphdr.src_port == tftp_req->port) {
		if (tp->opcode == 3) {
			if (tp->num == tftp_req->bloc || tp->num == tftp_req->bloc - 1) {
				_memset (&tftpd_pkt, 0, sizeof tftpd_pkt);
				tftpd_pkt.opcode = 4;
				tftpd_pkt.num = tp->num;
				for (nbtry = 4; nbtry; --nbtry) {
					if (sendUDP
						((UDPpkt *) & tftpd_pkt, 4, tftp_req->server,
						 TFTP_LOCAL_PORT, tp->udphdr.src_port) == 0)
						break;
					delay_msec(100);
				}
				if (nbtry == 0)
					printf ("\n\rTFTP: error sending ACK\n\r");
				if (tp->num == tftp_req->bloc) {
					register unsigned short len = tp->udphdr.length - 12;

					if (len) {
						_memcpy ((void *) (tftp_req->addr + tftp_req->bcnt),
								tp->data, len);
						tftp_req->bcnt += len;
					}
					++tftp_req->bloc;
				    if (0==(tftp_req->bloc&0x003F))
				        printf("#");
					if (tp->udphdr.length < 524) {
						tftp_req->sts = 1;
						printf ("\n\rTFTP: downloaded %d bytes\n\r",
								tftp_req->bcnt);
					}
				}
			}
		}
		else if (tp->opcode == 5) {
			printf ("\n\rTFTP error %d: %s\n\r", tp->num, tp->data);
			tftp_req->sts = -tp->num;
		}
	}
}

int sendTFTPRequest (void)
{
	TFTPRpkt tftpr_pkt;
	unsigned short len;

	if (tftp_req) {
		_memset (&tftpr_pkt, 0, sizeof tftpr_pkt);
		tftpr_pkt.opcode = 1;
		strcpy (tftpr_pkt.data, tftp_req->file);
		len = strlen (tftp_req->file) + 1;
		//strcpy (tftpr_pkt.data + len, "octet");
		_memcpy(tftpr_pkt.data + len, "octet", strlen("octet"));
		len += 8;				/* opcode size + octet\0 size */

		return sendUDP ((UDPpkt *) & tftpr_pkt, len, tftp_req->server,
						TFTP_LOCAL_PORT, TFTP_SRV_PORT);
	}
	return 1;
}
