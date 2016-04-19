/*-
 * <COPYRIGHT_TAG>
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
 * 4) The screens displayed by the application must contain the copyright notice as
 * defined above and can not be removed without specific prior written permission.
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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/queue.h>

#include <rte_common.h>
#include <rte_log.h>
#include <rte_debug.h>
#include <rte_memory.h>
#include <rte_memzone.h>
#include <rte_atomic.h>
#include <rte_launch.h>
#include <rte_tailq.h>
#include <rte_eal.h>
#include <rte_eal_memconfig.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_ring.h>
#include <rte_errno.h>
#include <rte_string_fns.h>
#include <rte_spinlock.h>
#include <rte_eal_memconfig.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>

#include "wr_mempool.h"

TAILQ_HEAD(rte_mempool_list, rte_mempool);

/*
 * return the greatest common divisor between a and b (fast algorithm)
 *
 */
static __inline__ unsigned
get_gcd(unsigned a, unsigned b)
{
	unsigned c;

	if (0 == a)
		return b;
	if (0 == b)
		return a;

	if (a < b) {
		c = a;
		a = b;
		b = c;
	}

	while (b != 0) {
		c = a % b;
		a = b;
		b = c;
	}

	return a;
}

/*
 * Depending on memory configuration, objects addresses are spreaded
 * between channels and ranks in RAM: the pool allocator will add
 * padding between objects. This function return the new size of the
 * object.
 */
static __inline__ unsigned
optimize_object_size(unsigned obj_size)
{
	unsigned nrank, nchan;
	unsigned new_obj_size;

	/* get number of channels */
	nchan = rte_memory_get_nchannel();
	if (nchan == 0)
		nchan = 1;

	nrank = rte_memory_get_nrank();
	if (nrank == 0)
		nrank = 1;

	/* process new object size */
	new_obj_size = (obj_size + CACHE_LINE_MASK) / CACHE_LINE_SIZE;
	while (get_gcd(new_obj_size, nrank * nchan) != 1 ||
			get_gcd(nchan, new_obj_size) != 1)
		new_obj_size++;
	return new_obj_size * CACHE_LINE_SIZE;
}

/* create the mempool with variable size buffers. */
struct rte_mempool *
rte_mempool_special_create(const char *name, unsigned n, unsigned mempool_data_size,
	unsigned private_data_size,
    rte_mempool_special_ctor_t *mp_init, rte_mempool_special_obj_ctor_t * special_obj_init,
    void *special_obj_init_arg, int socket_id, unsigned flags)
{
	char mz_name[RTE_MEMZONE_NAMESIZE];
	char rg_name[RTE_RING_NAMESIZE];
	struct rte_mempool *mp = NULL;
	struct rte_ring *r;
	const struct rte_memzone *mz;
	size_t mempool_size;
	int mz_flags = RTE_MEMZONE_1GB|RTE_MEMZONE_SIZE_HINT_ONLY;
	int rg_flags = 0;
	uint32_t header_size, trailer_size;
	unsigned i, buf_len = 0;
	void *obj;

    /* compilation-time checks */
    RTE_BUILD_BUG_ON((sizeof(struct rte_mempool) &
              CACHE_LINE_MASK) != 0);

    /* check that we have an initialised tail queue */
    if (RTE_TAILQ_LOOKUP_BY_IDX(RTE_TAILQ_MEMPOOL, rte_mempool_list) == NULL) {
        rte_errno = E_RTE_NO_TAILQ;
        return NULL;
    }

	/* "no cache align" imply "no spread" */
	if (flags & MEMPOOL_F_NO_CACHE_ALIGN)
		flags |= MEMPOOL_F_NO_SPREAD;

	/* ring flags */
	if (flags & MEMPOOL_F_SP_PUT)
		rg_flags |= RING_F_SP_ENQ;
	if (flags & MEMPOOL_F_SC_GET)
		rg_flags |= RING_F_SC_DEQ;

    rte_rwlock_write_lock(RTE_EAL_MEMPOOL_RWLOCK);

	/* allocate the ring that will be used to store objects */
	/* Ring functions will return appropriate errors if we are
	 * running as a secondary process etc., so no checks made
	 * in this function for that condition */
	snprintf(rg_name, sizeof(rg_name), "MP_%s", name);
	r = rte_ring_create(rg_name, rte_align32pow2(n+1), socket_id, rg_flags);
	if (r == NULL)
		goto exit;

	header_size	= 0;
	header_size += sizeof(struct rte_mempool *);
	if ((flags & MEMPOOL_F_NO_CACHE_ALIGN) == 0)
		header_size = (header_size + CACHE_LINE_MASK) & (~CACHE_LINE_MASK);

	trailer_size 		= 0;
	private_data_size	= (private_data_size +
			     CACHE_LINE_MASK) & (~CACHE_LINE_MASK);

	/* reserve a memory zone for this mempool: private data is cache-aligned */
	mempool_size = sizeof(struct rte_mempool) + private_data_size + mempool_data_size;

	snprintf(mz_name, sizeof(mz_name), "MP_%s", name);
	mz = rte_memzone_reserve(mz_name, mempool_size, socket_id, mz_flags);

	/*
	 * no more memory: in this case we loose previously reserved
	 * space for the as we cannot free it
	 */
	if (mz == NULL)
		goto exit;

	/* init the mempool structure */
	mp = mz->addr;
	memset(mp, 0, sizeof(*mp));
	snprintf(mp->name, sizeof(mp->name), "%s", name);
	mp->phys_addr			= mz->phys_addr;
	mp->ring				= r;
	mp->size				= n;
	mp->flags				= flags;
	mp->elt_size			= 0;
	mp->header_size			= header_size;
	mp->trailer_size		= trailer_size;
	mp->cache_size			= 0;
	mp->cache_flushthresh	= 0;
	mp->private_data_size	= private_data_size;

	/* call the initializer */
	if (mp_init)
		mp_init(mp, (void *)sizeof(struct rte_pktmbuf_pool_private));

	/* fill the headers and trailers, and add objects in ring */
	obj = (char *)mp + sizeof(struct rte_mempool) + private_data_size;
	for (i = 0; i < n; i++) {
		struct rte_mempool **mpp;
		obj = (char *)obj + header_size;

		/* set mempool ptr in header */
		mpp = __mempool_from_obj(obj);
		*mpp = mp;

		/* call the initializer */
		if (special_obj_init)
			buf_len = special_obj_init(mp, special_obj_init_arg, obj, i);

		/* enqueue in ring */
		rte_ring_sp_enqueue(mp->ring, obj);
		obj = (char *)obj + buf_len;
	}

    RTE_EAL_TAILQ_INSERT_TAIL(RTE_TAILQ_MEMPOOL, rte_mempool_list, mp);

exit:
    rte_rwlock_write_unlock(RTE_EAL_MEMPOOL_RWLOCK);

	return mp;
}
