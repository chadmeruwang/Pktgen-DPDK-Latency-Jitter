#   BSD LICENSE
# 
#   Copyright(c) 2010-2012 Intel Corporation. All rights reserved.
#   All rights reserved.
# 
#   Redistribution and use in source and binary forms, with or without 
#   modification, are permitted provided that the following conditions 
#   are met:
# 
#     * Redistributions of source code must retain the above copyright 
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright 
#       notice, this list of conditions and the following disclaimer in 
#       the documentation and/or other materials provided with the 
#       distribution.
#     * Neither the name of Intel Corporation nor the names of its 
#       contributors may be used to endorse or promote products derived 
#       from this software without specific prior written permission.
# 
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
#   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
#   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
#   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
#   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
#   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
#   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
#   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
#   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
#   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
#   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 

include $(RTE_SDK)/mk/rte.vars.mk

# library name
LIB = libcommon.a

CFLAGS += $(WERROR_FLAGS) -I$(SRCDIR) -O3 -I../scrn

# install includes
SYMLINK-$(CONFIG_WR_PKTGEN)-include := wr_copyright_info.h wr_port_config.h \
	wr_core_info.h wr_lscpu.h wr_utils.h wr_coremap.h wr_mempool.h wr_mbuf.h \
	wr_cycles.h wr_pcap.h wr_inet.h wr_cksum.h wr_l2p.h

# all source are stored in SRCS-y
SRCS-$(CONFIG_WR_PKTGEN) := wr_copyright_info.c wr_port_config.c wr_core_info.c \
	wr_lscpu.c wr_utils.c wr_coremap.c wr_mempool.c wr_pcap.c wr_cksum.c wr_l2p.c

include $(RTE_SDK)/mk/rte.lib.mk
