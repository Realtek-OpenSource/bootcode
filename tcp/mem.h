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
 * $Id: mem.h,v 1.2 2012/11/14 07:48:54 tctsai Exp $
 */
#ifndef __LWIP_MEM_H__
#define __LWIP_MEM_H__

#include "debug.h"
#include "opt.h"
#include "arch.h"

#if MEM_SIZE > 64000l
typedef u32_t mem_size_t;
#else
typedef u16_t mem_size_t;
#endif /* MEM_SIZE > 64000 */


void mem_init(void);

void *mem_malloc(mem_size_t size);
void *mem_malloc2(mem_size_t size);
void mem_free(void *mem);
void *mem_realloc(void *mem, mem_size_t size);
void *mem_reallocm(void *mem, mem_size_t size);

#ifdef MEM_PERF
void mem_perf_start(void);
void mem_perf_init(char *fname);
#endif /* MEM_PERF */

#ifdef MEM_RECLAIM
typedef mem_size_t (*mem_reclaim_func)(void *arg, mem_size_t size);
void mem_register_reclaim(mem_reclaim_func f, void *arg);
void mem_reclaim(unsigned int size);
#else
#define mem_register_reclaim(f, arg)
#endif /* MEM_RECLAIM */


#define MEM_ALIGN_SIZE(size) (size + \
                             ((((size) % MEM_ALIGNMENT) == 0)? 0 : \
                             (MEM_ALIGNMENT - ((size) % MEM_ALIGNMENT))))

#define MEM_ALIGN(addr) (void *)MEM_ALIGN_SIZE((u32_t)addr)

/*#define	MEM_SET_U16_T(ptr, data) { \
						*((unsigned char*)ptr)=((int)data>>24)&0xf; \
						*((unsigned char*)ptr+1)=((int)data>>16)&0xf; \
						*((unsigned char*)ptr+2)=((int)data>>8)&0xf; \
						*((unsigned char*)ptr+3)=((int)data)&0xf;}
*/
/*#define	MEM_GET_U16_T(ptr, data) {\
		data = ((*((unsigned char*)ptr))<<24)|((*((unsigned char*)ptr+1))<<16)|((*((unsigned char*)ptr+2))<<8)|(*((unsigned char*)ptr+3))}
*/

#endif /* __LWIP_MEM_H__ */

