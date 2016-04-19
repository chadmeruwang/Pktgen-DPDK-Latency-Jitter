/*-
 *   BSD LICENSE
 * 
 *   Copyright(c) 2010-2012 Intel Corporation. All rights reserved.
 *   All rights reserved.
 * 
 *   Redistribution and use in source and binary forms, with or without 
 *   modification, are permitted provided that the following conditions 
 *   are met:
 * 
 *     * Redistributions of source code must retain the above copyright 
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright 
 *       notice, this list of conditions and the following disclaimer in 
 *       the documentation and/or other materials provided with the 
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its 
 *       contributors may be used to endorse or promote products derived 
 *       from this software without specific prior written permission.
 * 
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef _WR_MEMPOOL_H_
#define _WR_MEMPOOL_H_

/**
 * @file
 * RTE Mempool.
 *
 * A memory pool is an allocator of fixed-size object. It is
 * identified by its name, and uses a ring to store free objects. It
 * provides some other optional services, like a per-core object
 * cache, and an alignment helper to ensure that objects are padded
 * to spread them equally on all RAM channels, ranks, and so on.
 *
 * Objects owned by a mempool should never be added in another
 * mempool. When an object is freed using rte_mempool_put() or
 * equivalent, the object data is not modified; the user can save some
 * meta-data in the object data and retrieve them when allocating a
 * new object.
 *
 * Note: the mempool implementation is not preemptable. A lcore must
 * not be interrupted by another task that uses the same mempool
 * (because it uses a ring which is not preemptable). Also, mempool
 * functions must not be used outside the DPDK environment: for
 * example, in linuxapp environment, a thread that is not created by
 * the EAL must not use mempools. This is due to the per-lcore cache
 * that won't work as rte_lcore_id() will not return a correct value.
 */

#include <rte_mempool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * An object constructor callback function for mempool.
 *
 * Arguments are the mempool, the opaque pointer given by the user in
 * rte_mempool_create(), the pointer to the element and the index of
 * the element in the pool.
 */
typedef uint32_t (rte_mempool_special_obj_ctor_t)(struct rte_mempool *, void *,
				      void *, unsigned);

/**
 * A mempool constructor callback function.
 *
 * Arguments are the mempool and the opaque pointer given by the user in
 * rte_mempool_create().
 */
typedef void (rte_mempool_special_ctor_t)(struct rte_mempool *, void *);

/**
 * Creates a new mempool named *name* in memory.
 *
 * This function uses ``memzone_reserve()`` to allocate memory. The
 * pool contains n elements of elt_size. Its size is set to n.
 *
 * @param name
 *   The name of the mempool.
 * @param n
 *   The number of elements in the mempool. The optimum size (in terms of
 *   memory usage) for a mempool is when n is a power of two minus one:
 *   n = (2^q - 1).
 * @param mempool_data_size
 *   Size of mempool data size.
 * @param private_data_size
 *   The size of the private data appended after the mempool
 *   structure. This is useful for storing some private data after the
 *   mempool structure, as is done for rte_mbuf_pool for example.
 * @param mp_init
 *   A function pointer that is called for initialization of the pool,
 *   before object initialization. The user can initialize the private
 *   data in this function if needed. This parameter can be NULL if
 *   not needed.
 * @param mp_init_arg
 *   An opaque pointer to data that can be used in the mempool
 *   constructor function.
 * @param obj_init
 *   A function pointer that is called for each object at
 *   initialization of the pool. The user can set some meta data in
 *   objects if needed. This parameter can be NULL if not needed.
 *   The obj_init() function takes the mempool pointer, the init_arg,
 *   the object pointer and the object number as parameters.
 * @param obj_init_arg
 *   An opaque pointer to data that can be used as an argument for
 *   each call to the object constructor function.
 * @param socket_id
 *   The *socket_id* argument is the socket identifier in the case of
 *   NUMA. The value can be *SOCKET_ID_ANY* if there is no NUMA
 *   constraint for the reserved zone.
 * @param flags
 *   The *flags* arguments is an OR of following flags:
 *   - MEMPOOL_F_SP_PUT: If this flag is set, the default behavior
 *     when using rte_mempool_put() or rte_mempool_put_bulk() is
 *     "single-producer". Otherwise, it is "multi-producers".
 *   - MEMPOOL_F_SC_GET: If this flag is set, the default behavior
 *     when using rte_mempool_get() or rte_mempool_get_bulk() is
 *     "single-consumer". Otherwise, it is "multi-consumers".
 * @return
 *   The pointer to the new allocated mempool, on success. NULL on error
 *   with rte_errno set appropriately. Possible rte_errno values include:
 *    - E_RTE_NO_CONFIG - function could not get pointer to rte_config structure
 *    - E_RTE_SECONDARY - function was called from a secondary process instance
 *    - E_RTE_NO_TAILQ - no tailq list could be got for the ring or mempool list
 *    - EINVAL - cache size provided is too large
 *    - ENOSPC - the maximum number of memzones has already been allocated
 *    - EEXIST - a memzone with the same name already exists
 *    - ENOMEM - no appropriate memory area found in which to create memzone
 */
struct rte_mempool *
rte_mempool_special_create(const char *name, unsigned n, unsigned mempool_data_size,
		   unsigned private_data_size,
		   rte_mempool_special_ctor_t *mp_init, rte_mempool_special_obj_ctor_t *obj_init,
		   void *special_obj_init_arg, int socket_id, unsigned flags);

/* Create the element size from the packet length */
static __inline__ uint32_t
wr_mempool_element_size( uint32_t elt_size )
{
	elt_size = (elt_size + CACHE_LINE_MASK) & ~CACHE_LINE_MASK;

	return (elt_size + (CACHE_LINE_SIZE + sizeof(struct rte_mbuf) + RTE_PKTMBUF_HEADROOM));
}

#ifdef __cplusplus
}
#endif

#endif /* _WR_MEMPOOL_H_ */
