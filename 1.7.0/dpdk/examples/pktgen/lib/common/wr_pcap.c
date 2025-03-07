/*-
 * Copyright (c) <2010>, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 *
 * - Neither the name of Intel Corporation nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * Copyright (c) <2010-2014>, Wind River Systems, Inc.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 * 1) Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3) Neither the name of Wind River Systems nor the names of its contributors may be
 * used to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * 4) The screens displayed by the application must contain the copyright notice as defined
 * above and can not be removed without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/* Created 2011 by Keith Wiles @ windriver.com */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/queue.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <assert.h>
#include <netinet/in.h>

#include <rte_version.h>
#include <rte_config.h>

#include <rte_log.h>
#include <rte_tailq.h>
#include <rte_tailq_elem.h>
#include <rte_common.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include <rte_memzone.h>
#include <rte_malloc.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_launch.h>
#include <rte_atomic.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_pci.h>
#include <rte_random.h>
#include <rte_timer.h>
#include <rte_debug.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_ring.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <rte_hash.h>
#include <rte_lpm.h>
#include <rte_string_fns.h>
#include <rte_scrn.h>
#include <rte_byteorder.h>
#include <rte_spinlock.h>
#include <rte_errno.h>

#include "wr_pcap.h"
#include "wr_inet.h"

/**************************************************************************//**
*
* wr_pcap_open - Open a PCAP file.
*
* DESCRIPTION
* Open a PCAP file to be used in sending from a port.
*
* RETURNS: N/A
*
* SEE ALSO:
*/

pcap_info_t *
wr_pcap_open(char * filename, uint16_t port)
{
	pcap_info_t	  * pcap = NULL;
	
	if ( filename == NULL ) {
		printf("%s: filename is NULL\n", __FUNCTION__);
		goto leave;
	}
	
	pcap = (pcap_info_t *)rte_malloc("PCAP info", sizeof(pcap_info_t), CACHE_LINE_SIZE);
	if ( pcap == NULL ) {
		printf("%s: malloc failed for pcap_info_t structure\n", __FUNCTION__);
		goto leave;
	}
	memset((char *)pcap, 0, sizeof(pcap_info_t));
	
	pcap->fd = fopen((const char *)filename, "r");
	if ( pcap->fd == NULL ) {
		printf("%s: failed for (%s)\n", __FUNCTION__, filename);
		goto leave;
	}
	
	if ( fread(&pcap->info, 1, sizeof(pcap_hdr_t), pcap->fd) != sizeof(pcap_hdr_t) ) {
		printf("%s: failed to read the file header\n", __FUNCTION__);
		goto leave;
	}

	// Default to little endian format.
	pcap->endian	= LITTLE_ENDIAN;
	pcap->filename	= strdup(filename);

	// Make sure we have a valid PCAP file for Big or Little Endian formats.
	if ( (pcap->info.magic_number != PCAP_MAGIC_NUMBER) &&
		 (pcap->info.magic_number != ntohl(PCAP_MAGIC_NUMBER)) ) {
		printf("%s: Magic Number does not match!\n", __FUNCTION__); fflush(stdout);
		goto leave;
	}

	// Convert from big-endian to little-endian.
	if ( pcap->info.magic_number == ntohl(PCAP_MAGIC_NUMBER) ) {
		printf("PCAP: Big Endian file format found, converting to little endian\n");
		pcap->endian				= BIG_ENDIAN;
		pcap->info.magic_number 	= ntohl(pcap->info.magic_number);
		pcap->info.network			= ntohl(pcap->info.network);
		pcap->info.sigfigs			= ntohl(pcap->info.sigfigs);
		pcap->info.snaplen			= ntohl(pcap->info.snaplen);
		pcap->info.thiszone			= ntohl(pcap->info.thiszone);
		pcap->info.version_major	= ntohs(pcap->info.version_major);
		pcap->info.version_minor	= ntohs(pcap->info.version_minor);
	}
	wr_pcap_info(pcap, port, 0);

	return pcap;
	
leave:
	wr_pcap_close(pcap);
	fflush(stdout);

	return NULL;
}

 /**************************************************************************//**
 *
 * wr_pcap_info - Display the PCAP information.
 *
 * DESCRIPTION
 * Dump out the PCAP information.
 *
 * RETURNS: N/A
 *
 * SEE ALSO:
 */

void
wr_pcap_info(pcap_info_t * pcap, uint16_t port, int flag)
{
	printf("\nPCAP file for port %d: %s\n", port, pcap->filename);
	printf("  magic: %08x,", pcap->info.magic_number);
	printf(" Version: %d.%d,", pcap->info.version_major, pcap->info.version_minor);
	printf(" Zone: %d,", pcap->info.thiszone);
	printf(" snaplen: %d,", pcap->info.snaplen);
	printf(" sigfigs: %d,", pcap->info.sigfigs);
	printf(" network: %d", pcap->info.network);
	printf(" Endian: %s\n", pcap->endian == BIG_ENDIAN ? "Big" : "Little");
	if ( flag )
		printf("  Packet count: %d\n", pcap->pkt_count);
	printf("\n");
	fflush(stdout);
}

/**************************************************************************//**
*
* wr_pcap_rewind - Rewind or start over on a PCAP file.
*
* DESCRIPTION
* Rewind or start over on a PCAP file.
*
* RETURNS: N/A
*
* SEE ALSO:
*/

void
wr_pcap_rewind(pcap_info_t * pcap)
{
	if ( pcap == NULL )
		return;

	// Rewind to the beginning
	rewind(pcap->fd);

	// Seek past the pcap header
	(void)fseek(pcap->fd, sizeof(pcap_hdr_t), SEEK_SET);
}

/**************************************************************************//**
*
* wr_pcap_skip - Rewind and skip to the given packet location.
*
* DESCRIPTION
* Rewind and skip to the given packet location.
*
* RETURNS: N/A
*
* SEE ALSO:
*/

void
wr_pcap_skip(pcap_info_t * pcap, uint32_t skip)
{
	pcaprec_hdr_t	hdr, * phdr;

	if ( pcap == NULL )
		return;

	// Rewind to the beginning
	rewind(pcap->fd);

	// Seek past the pcap header
	(void)fseek(pcap->fd, sizeof(pcap_hdr_t), SEEK_SET);

	phdr = &hdr;
	while( skip-- ) {
		if ( fread(phdr, 1, sizeof(pcaprec_hdr_t), pcap->fd) != sizeof(pcaprec_hdr_t) )
			break;

		// Convert the packet header to the correct format.
		wr_pcap_convert(pcap, phdr);

		(void)fseek(pcap->fd, phdr->incl_len, SEEK_CUR);
	}
}

/**************************************************************************//**
*
* wr_pcap_close - Close a PCAP file
*
* DESCRIPTION
* Close the PCAP file for sending.
*
* RETURNS: N/A
*
* SEE ALSO:
*/

void
wr_pcap_close(pcap_info_t * pcap)
{
	if ( pcap == NULL )
		return;

	if ( pcap->fd )
		fclose(pcap->fd);
	if ( pcap->filename )
		free(pcap->filename);
	rte_free(pcap);
}

/**************************************************************************//**
*
* wr_payloadOffset - Determine the packet data offset value.
*
* DESCRIPTION
* Determine the packet data offset value in bytes.
*
* RETURNS: N/A
*
* SEE ALSO:
*/

int
wr_payloadOffset(const unsigned char *pkt_data, unsigned int *offset,
                          unsigned int *length) {
    const ipHdr_t *iph = (const ipHdr_t *)(pkt_data + sizeof(struct ether_hdr));
    const tcpHdr_t *th = NULL;

    // Ignore packets that aren't IPv4
    if ( (iph->vl & 0xF0) != 0x40)
        return -1;

    // Ignore fragmented packets.
    if (iph->ffrag & htons(PG_OFF_MF|PG_OFF_MASK))
        return -1;

    // IP header length, and transport header length.
    unsigned int ihlen = (iph->vl & 0x0F) * 4;
    unsigned int thlen = 0;

    switch (iph->proto) {
    case PG_IPPROTO_TCP:
        th = (const tcpHdr_t *)((const char *)iph + ihlen);
        thlen = (th->offset >> 4) * 4;
        break;
    case PG_IPPROTO_UDP:
        thlen = sizeof(udpHdr_t);
        break;
    default:
        return -1;
    }

    *offset = sizeof(struct ether_hdr) + ihlen + thlen;
    *length = sizeof(struct ether_hdr) + ntohs(iph->tlen) - *offset;

    return *length != 0;
}

/**************************************************************************//**
*
* wr_pcap_read - Read data from the PCAP file and parse it
*
* DESCRIPTION
* Parse the data from the PCAP file.
*
* RETURNS: N/A
*
* SEE ALSO:
*/

size_t
wr_pcap_read(pcap_info_t * pcap, pcaprec_hdr_t * pHdr, char * pktBuff, uint32_t bufLen )
{
	do {
		if ( fread(pHdr, 1, sizeof(pcaprec_hdr_t), pcap->fd) != sizeof(pcaprec_hdr_t) )
			return 0;

		// Convert the packet header to the correct format.
		wr_pcap_convert(pcap, pHdr);

		// Skip packets larger then the buffer size.
		if ( pHdr->incl_len > bufLen ) {
			(void)fseek(pcap->fd, pHdr->incl_len, SEEK_CUR);
			return pHdr->incl_len;
		}
	
		return fread(pktBuff, 1, pHdr->incl_len, pcap->fd);
	} while(1);
}
