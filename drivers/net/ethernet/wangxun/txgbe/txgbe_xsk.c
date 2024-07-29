// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 1999 - 2022 Intel Corporation. */

#include "txgbe.h"

#ifdef HAVE_XDP_SUPPORT
#include <linux/bpf_trace.h>
#endif
#ifdef HAVE_AF_XDP_ZC_SUPPORT
#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
#include <net/xdp_sock.h>
#else
#include <net/xdp_sock_drv.h>
#endif
#endif
#ifdef HAVE_XDP_BUFF_RXQ
#include <net/xdp.h>
#endif

#include "txgbe_xsk.h"
#ifdef HAVE_AF_XDP_ZC_SUPPORT
static void txgbe_disable_txr_hw(struct txgbe_adapter *adapter,
				 struct txgbe_ring *tx_ring)
{
	struct txgbe_hw *hw = &adapter->hw;
	u8 reg_idx = tx_ring->reg_idx;
	u32 txdctl;

	wr32(hw, TXGBE_PX_TR_CFG(reg_idx), TXGBE_PX_TR_CFG_SWFLSH);;

	/* delay mechanism from txgbe_disable_tx */
	msleep(10);
	
	txdctl = rd32(hw, TXGBE_PX_TR_CFG(reg_idx));
	
	if (!(txdctl & TXGBE_PX_TR_CFG_ENABLE))
		return;

	e_err(drv, "TXDCTL.ENABLE not cleared within the polling period\n");
}


static void txgbe_disable_rxr_hw(struct txgbe_adapter *adapter,
				 struct txgbe_ring *rx_ring)
{
	struct txgbe_hw *hw = &adapter->hw;
	u8 reg_idx = rx_ring->reg_idx;
	u32 rxdctl;

	rxdctl = rd32(hw, TXGBE_PX_RR_CFG(reg_idx));
	rxdctl &= ~TXGBE_PX_RR_CFG_RR_EN;

	/* write value back with RXDCTL.ENABLE bit cleared */
	wr32(hw, TXGBE_PX_RR_CFG(reg_idx), rxdctl);
	TXGBE_WRITE_FLUSH(hw);
	
	msleep(10);
	rxdctl = rd32(hw, TXGBE_PX_RR_CFG(reg_idx));
	
	if (!(rxdctl & TXGBE_PX_RR_CFG_RR_EN))
		return;

	e_err(drv, "RXDCTL.ENABLE not cleared within the polling period\n");
}

static void txgbe_disable_txr(struct txgbe_adapter *adapter,
			      struct txgbe_ring *tx_ring)
{
	set_bit(__TXGBE_TX_DISABLED, &tx_ring->state);
	txgbe_disable_txr_hw(adapter, tx_ring);
}


static void txgbe_reset_txr_stats(struct txgbe_ring *tx_ring)
{
	memset(&tx_ring->stats, 0, sizeof(tx_ring->stats));
	memset(&tx_ring->tx_stats, 0, sizeof(tx_ring->tx_stats));
}

static void txgbe_reset_rxr_stats(struct txgbe_ring *rx_ring)
{
	memset(&rx_ring->stats, 0, sizeof(rx_ring->stats));
	memset(&rx_ring->rx_stats, 0, sizeof(rx_ring->rx_stats));
}

/**
 * txgbe_txrx_ring_disable - Disable Rx/Tx/XDP Tx rings
 * @adapter: adapter structure
 * @ring: ring index
 *
 * This function disables a certain Rx/Tx/XDP Tx ring. The function
 * assumes that the netdev is running.
 **/
void txgbe_txrx_ring_disable(struct txgbe_adapter *adapter, int ring)
{
	struct txgbe_ring *rx_ring, *tx_ring, *xdp_ring;

	rx_ring = adapter->rx_ring[ring];
	tx_ring = adapter->tx_ring[ring];
	xdp_ring = adapter->xdp_ring[ring];

	txgbe_disable_txr(adapter, tx_ring);
	if (xdp_ring)
		txgbe_disable_txr(adapter, xdp_ring);
	txgbe_disable_rxr_hw(adapter, rx_ring);

	if (ring_is_xdp(tx_ring))
		synchronize_rcu();

	/* Rx/Tx/XDP Tx share the same napi context. */
	napi_disable(&rx_ring->q_vector->napi);

	txgbe_clean_tx_ring(tx_ring);
	if (xdp_ring)
		txgbe_clean_tx_ring(xdp_ring);
	txgbe_clean_rx_ring(rx_ring);

	txgbe_reset_txr_stats(tx_ring);
	if (xdp_ring)
		txgbe_reset_txr_stats(xdp_ring);
	txgbe_reset_rxr_stats(rx_ring);
}

/**
 * txgbe_txrx_ring_enable - Enable Rx/Tx/XDP Tx rings
 * @adapter: adapter structure
 * @ring: ring index
 *
 * This function enables a certain Rx/Tx/XDP Tx ring. The function
 * assumes that the netdev is running.
 **/
void txgbe_txrx_ring_enable(struct txgbe_adapter *adapter, int ring)
{
	struct txgbe_ring *rx_ring, *tx_ring, *xdp_ring;

	rx_ring = adapter->rx_ring[ring];
	tx_ring = adapter->tx_ring[ring];
	xdp_ring = adapter->xdp_ring[ring];

	/* Rx/Tx/XDP Tx share the same napi context. */
	napi_enable(&rx_ring->q_vector->napi);

	txgbe_configure_tx_ring(adapter, tx_ring);
	if (xdp_ring)
		txgbe_configure_tx_ring(adapter, xdp_ring);
	txgbe_configure_rx_ring(adapter, rx_ring);

	clear_bit(__TXGBE_TX_DISABLED, &tx_ring->state);
	if (ring_is_xdp(tx_ring))
		clear_bit(__TXGBE_TX_DISABLED, &xdp_ring->state);
}

#ifndef HAVE_NETDEV_BPF_XSK_POOL
struct xdp_umem *txgbe_xsk_umem(struct txgbe_adapter *adapter,
				struct txgbe_ring *ring)
#else
struct xsk_buff_pool *txgbe_xsk_umem(struct txgbe_adapter *adapter,
				     struct txgbe_ring *ring)
#endif /* HAVE_NETDEV_BPF_XSK_POOL */
{
	bool xdp_on = READ_ONCE(adapter->xdp_prog);
	int qid = ring->queue_index;

	if (!adapter->xsk_pools || !adapter->xsk_pools[qid] ||
	    qid >= adapter->num_xsk_pools || !xdp_on ||
	    !test_bit(qid, adapter->af_xdp_zc_qps))
		return NULL;
	return adapter->xsk_pools[qid];
}

static int txgbe_alloc_xsk_umems(struct txgbe_adapter *adapter)
{
	if (adapter->xsk_pools)
		return 0;

	adapter->num_xsk_pools_used = 0;
	adapter->num_xsk_pools = adapter->num_rx_queues;
	adapter->xsk_pools = kcalloc(adapter->num_xsk_pools,
				     sizeof(*adapter->xsk_pools),
				     GFP_KERNEL);
	if (!adapter->xsk_pools) {
		adapter->num_xsk_pools = 0;
		return -ENOMEM;
	}

	return 0;
}

/**
 * txgbe_xsk_any_rx_ring_enabled - Checks if Rx rings have AF_XDP UMEM attached
 * @adapter: adapter
 *
 * Returns true if any of the Rx rings has an AF_XDP UMEM attached
 **/
bool txgbe_xsk_any_rx_ring_enabled(struct txgbe_adapter *adapter)
{
	int i;

	if (!adapter->xsk_pools)
		return false;

	for (i = 0; i < adapter->num_xsk_pools; i++) {
		if (adapter->xsk_pools[i])
			return true;
	}

	return false;
}

#ifndef HAVE_NETDEV_BPF_XSK_POOL
static int txgbe_add_xsk_umem(struct txgbe_adapter *adapter,
			      struct xdp_umem *pool,
			      u16 qid)
#else
static int txgbe_add_xsk_umem(struct txgbe_adapter *adapter,
			      struct xsk_buff_pool *pool,
			      u16 qid)
#endif
{
	int err;

	err = txgbe_alloc_xsk_umems(adapter);
	if (err)
		return err;

	adapter->xsk_pools[qid] = pool;
	adapter->num_xsk_pools_used++;

	return 0;
}

static void txgbe_remove_xsk_umem(struct txgbe_adapter *adapter, u16 qid)
{
	adapter->xsk_pools[qid] = NULL;
	adapter->num_xsk_pools_used--;

	if (adapter->num_xsk_pools == 0) {
		kfree(adapter->xsk_pools);
		adapter->xsk_pools = NULL;
		adapter->num_xsk_pools = 0;
	}
}

#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
static int txgbe_xsk_umem_dma_map(struct txgbe_adapter *adapter,
				  struct xdp_umem *pool)
{
	struct device *dev = &adapter->pdev->dev;
	unsigned int i, j;
	dma_addr_t dma;

	for (i = 0; i < pool->npgs; i++) {
		dma = dma_map_page_attrs(dev, pool->pgs[i], 0, PAGE_SIZE,
					 DMA_BIDIRECTIONAL, TXGBE_RX_DMA_ATTR);
		if (dma_mapping_error(dev, dma))
			goto out_unmap;

		pool->pages[i].dma = dma;
	}

	return 0;

out_unmap:
	for (j = 0; j < i; j++) {
		dma_unmap_page_attrs(dev, pool->pages[i].dma, PAGE_SIZE,
				     DMA_BIDIRECTIONAL, TXGBE_RX_DMA_ATTR);
		pool->pages[i].dma = 0;
	}

	return -1;
}

static void txgbe_xsk_umem_dma_unmap(struct txgbe_adapter *adapter,
				     struct xdp_umem *pool)
{
	struct device *dev = &adapter->pdev->dev;
	unsigned int i;

	for (i = 0; i < pool->npgs; i++) {
		dma_unmap_page_attrs(dev, pool->pages[i].dma, PAGE_SIZE,
				     DMA_BIDIRECTIONAL, TXGBE_RX_DMA_ATTR);

		pool->pages[i].dma = 0;
	}
}
#endif /* HAVE_MEM_TYPE_XSK_BUFF_POOL */

#ifndef HAVE_NETDEV_BPF_XSK_POOL
static int txgbe_xsk_umem_enable(struct txgbe_adapter *adapter,
				 struct xdp_umem *pool,
				 u16 qid)
#else
static int txgbe_xsk_umem_enable(struct txgbe_adapter *adapter,
				 struct xsk_buff_pool *pool,
				 u16 qid)
#endif
{
#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
	struct xdp_umem_fq_reuse *reuseq;
#endif
	bool if_running;
	int err;

	if (qid >= adapter->num_rx_queues)
		return -EINVAL;

	if (adapter->xsk_pools) {
		if (qid >= adapter->num_xsk_pools)
			return -EINVAL;
		if (adapter->xsk_pools[qid])
			return -EBUSY;
	}

#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
	reuseq = xsk_reuseq_prepare(adapter->rx_ring[0]->count);
	if (!reuseq)
		return -ENOMEM;

	xsk_reuseq_free(xsk_reuseq_swap(pool, reuseq));

	err = txgbe_xsk_umem_dma_map(adapter, pool);
#else
	err = xsk_pool_dma_map(pool, &adapter->pdev->dev, TXGBE_RX_DMA_ATTR);
#endif /* HAVE_MEM_TYPE_XSK_BUFF_POOL */
	if (err)
		return err;

	if_running = netif_running(adapter->netdev) &&
		     READ_ONCE(adapter->xdp_prog);

	if (if_running)
		txgbe_txrx_ring_disable(adapter, qid);

	set_bit(qid, adapter->af_xdp_zc_qps);
	err = txgbe_add_xsk_umem(adapter, pool, qid);
	if (err)
		return err;

	if (if_running) {
		txgbe_txrx_ring_enable(adapter, qid);
		
		/* Kick start the NAPI context so that receiving will start */
#ifdef HAVE_NDO_XSK_WAKEUP
		err = txgbe_xsk_wakeup(adapter->netdev, qid, XDP_WAKEUP_RX);
#else
		err = txgbe_xsk_async_xmit(adapter->netdev, qid);
#endif
		
		if (err)
			return err;
	}

	return 0;
}

static int txgbe_xsk_umem_disable(struct txgbe_adapter *adapter, u16 qid)
{
	bool if_running;

	if (!adapter->xsk_pools || qid >= adapter->num_xsk_pools ||
	    !adapter->xsk_pools[qid])
		return -EINVAL;

	if_running = netif_running(adapter->netdev) &&
		     READ_ONCE(adapter->xdp_prog);

	if (if_running)
		txgbe_txrx_ring_disable(adapter, qid);

	clear_bit(qid, adapter->af_xdp_zc_qps);

#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
	txgbe_xsk_umem_dma_unmap(adapter, adapter->xsk_pools[qid]);
#else
	xsk_pool_dma_unmap(adapter->xsk_pools[qid], TXGBE_RX_DMA_ATTR);
#endif /* HAVE_MEM_TYPE_XSK_BUFF_POOL */
	txgbe_remove_xsk_umem(adapter, qid);

	if (if_running)
		txgbe_txrx_ring_enable(adapter, qid);

	return 0;
}

#ifndef HAVE_NETDEV_BPF_XSK_POOL
int txgbe_xsk_umem_setup(struct txgbe_adapter *adapter, struct xdp_umem *pool,
			 u16 qid)
#else
int txgbe_xsk_umem_setup(struct txgbe_adapter *adapter, struct xsk_buff_pool *pool,
			 u16 qid)
#endif
{
	return pool ? txgbe_xsk_umem_enable(adapter, pool, qid) :
		txgbe_xsk_umem_disable(adapter, qid);
}

static int txgbe_run_xdp_zc(struct txgbe_adapter *adapter,
			    struct txgbe_ring *rx_ring,
			    struct xdp_buff *xdp)
{
	int err, result = TXGBE_XDP_PASS;
	struct bpf_prog *xdp_prog;
	struct txgbe_ring *ring;
	struct xdp_frame *xdpf;
	u32 act;

	rcu_read_lock();
	xdp_prog = READ_ONCE(rx_ring->xdp_prog);
	act = bpf_prog_run_xdp(xdp_prog, xdp);
#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
	xdp->handle += xdp->data - xdp->data_hard_start;
#endif
	switch (act) {
	case XDP_PASS:
		break;
	case XDP_TX:
		xdpf = xdp_convert_buff_to_frame(xdp);
		if (unlikely(!xdpf)) {
			result = TXGBE_XDP_CONSUMED;
			break;
		}
		ring = adapter->xdp_ring[smp_processor_id() % MAX_XDP_QUEUES];
		if (static_branch_unlikely(&txgbe_xdp_locking_key))
			spin_lock(&ring->tx_lock);
#ifdef HAVE_XDP_FRAME_STRUCT
		result = txgbe_xmit_xdp_ring(ring, xdpf);
#else
		result = txgbe_xmit_xdp_ring(ring, xdp);
#endif /* HAVE_XDP_FRAME_STRUCT */
		if (static_branch_unlikely(&txgbe_xdp_locking_key))
			spin_unlock(&ring->tx_lock);
		break;
	case XDP_REDIRECT:
		err = xdp_do_redirect(rx_ring->netdev, xdp, xdp_prog);
		result = !err ? TXGBE_XDP_REDIR : TXGBE_XDP_CONSUMED;
		break;
	default:
		bpf_warn_invalid_xdp_action(rx_ring->netdev, xdp_prog, act);
		fallthrough;
	case XDP_ABORTED:
		trace_xdp_exception(rx_ring->netdev, xdp_prog, act);
		/* fallthrough -- handle aborts by dropping packet */
		fallthrough;
	case XDP_DROP:
		result = TXGBE_XDP_CONSUMED;
		break;
	}
	rcu_read_unlock();
	return result;
}

#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
static struct
txgbe_rx_buffer *txgbe_get_rx_buffer_zc(struct txgbe_ring *rx_ring,
					unsigned int size)
{
	struct txgbe_rx_buffer *bi;

	bi = &rx_ring->rx_buffer_info[rx_ring->next_to_clean];

	/* we are reusing so sync this buffer for CPU use */
	dma_sync_single_range_for_cpu(rx_ring->dev,
				      bi->dma, 0,
				      size,
				      DMA_BIDIRECTIONAL);

	return bi;
}

static void txgbe_reuse_rx_buffer_zc(struct txgbe_ring *rx_ring,
				     struct txgbe_rx_buffer *obi)
{
	unsigned long mask = (unsigned long)rx_ring->xsk_pool->chunk_mask;
	u64 hr = rx_ring->xsk_pool->headroom + XDP_PACKET_HEADROOM;
	u16 nta = rx_ring->next_to_alloc;
	struct txgbe_rx_buffer *nbi;

	nbi = &rx_ring->rx_buffer_info[rx_ring->next_to_alloc];
	/* update, and store next to alloc */
	nta++;
	rx_ring->next_to_alloc = (nta < rx_ring->count) ? nta : 0;

	/* transfer page from old buffer to new buffer */
	nbi->dma = obi->dma & mask;
	nbi->dma += hr;

	nbi->addr = (void *)((unsigned long)obi->addr & mask);
	nbi->addr += hr;

	nbi->handle = obi->handle & mask;
	nbi->handle += rx_ring->xsk_pool->headroom;

	obi->addr = NULL;
	obi->skb = NULL;
}

#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
void txgbe_zca_free(struct zero_copy_allocator *alloc, unsigned long handle)
{
	struct txgbe_rx_buffer *bi;
	struct txgbe_ring *rx_ring;
	u64 hr, mask;
	u16 nta;

	rx_ring = container_of(alloc, struct txgbe_ring, zca);
	hr = rx_ring->xsk_pool->headroom + XDP_PACKET_HEADROOM;
	mask = rx_ring->xsk_pool->chunk_mask;

	nta = rx_ring->next_to_alloc;
	bi = rx_ring->rx_buffer_info;

	nta++;
	rx_ring->next_to_alloc = (nta < rx_ring->count) ? nta : 0;

	handle &= mask;

	bi->dma = xdp_umem_get_dma(rx_ring->xsk_pool, handle);
	bi->dma += hr;

	bi->addr = xdp_umem_get_data(rx_ring->xsk_pool, handle);
	bi->addr += hr;

	bi->handle = (u64)handle + rx_ring->xsk_pool->headroom;
}
#endif

static bool txgbe_alloc_buffer_zc(struct txgbe_ring *rx_ring,
				  struct txgbe_rx_buffer *bi)
{
#ifndef HAVE_NETDEV_BPF_XSK_POOL
	struct xdp_umem *umem = rx_ring->xsk_pool;
#endif
	void *addr = bi->addr;
	u64 handle, hr;

	if (addr)
		return true;

	if (!xsk_umem_peek_addr(umem, &handle)) {
		rx_ring->rx_stats.alloc_rx_page_failed++;
		return false;
	}

	hr = umem->headroom + XDP_PACKET_HEADROOM;

	bi->dma = xdp_umem_get_dma(umem, handle);
	bi->dma += hr;

	bi->addr = xdp_umem_get_data(umem, handle);
	bi->addr += hr;

	bi->handle = handle + umem->headroom;

	xsk_umem_release_addr(umem);
	return true;
}

static bool txgbe_alloc_buffer_slow_zc(struct txgbe_ring *rx_ring,
				       struct txgbe_rx_buffer *bi)
{
#ifndef HAVE_NETDEV_BPF_XSK_POOL
	struct xdp_umem *pool = rx_ring->xsk_pool;
#else
	struct xsk_buff_pool *pool = rx_ring->xsk_pool;
#endif
	u64 handle, hr;

	if (!xsk_umem_peek_addr_rq(pool, &handle)) {
		rx_ring->rx_stats.alloc_rx_page_failed++;
		return false;
	}

	handle &= rx_ring->xsk_pool->chunk_mask;

	hr = pool->headroom + XDP_PACKET_HEADROOM;

	bi->dma = xdp_umem_get_dma(pool, handle);
	bi->dma += hr;

	bi->addr = xdp_umem_get_data(pool, handle);
	bi->addr += hr;

	bi->handle = handle + pool->headroom;

	xsk_umem_release_addr_rq(pool);
	return true;
}

static __always_inline bool
__txgbe_alloc_rx_buffers_zc(struct txgbe_ring *rx_ring, u16 count,
			    bool alloc(struct txgbe_ring *rx_ring,
				       struct txgbe_rx_buffer *bi))
#else
bool txgbe_alloc_rx_buffers_zc(struct txgbe_ring *rx_ring, u16 count)
#endif /* HAVE_MEM_TYPE_XSK_BUFF_POOL */
{
	union txgbe_rx_desc *rx_desc;
	struct txgbe_rx_buffer *bi;
	u16 i = rx_ring->next_to_use;
#ifdef HAVE_MEM_TYPE_XSK_BUFF_POOL
	dma_addr_t dma;
#endif
	bool ok = true;

	/* nothing to do */
	if (!count)
		return true;

	rx_desc = TXGBE_RX_DESC(rx_ring, i);
	bi = &rx_ring->rx_buffer_info[i];
	i -= rx_ring->count;

	do {
#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
		if (!alloc(rx_ring, bi)) {
#else
		bi->xdp = xsk_buff_alloc(rx_ring->xsk_pool);
		if (!bi->xdp) {
#endif
			ok = false;
			break;
		}

#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
		/* sync the buffer for use by the device */
		dma_sync_single_range_for_device(rx_ring->dev, bi->dma,
						 bi->page_offset,
						 rx_ring->rx_buf_len,
						 DMA_BIDIRECTIONAL);
#else
		dma = xsk_buff_xdp_get_dma(bi->xdp);
#endif

		/* Refresh the desc even if buffer_addrs didn't change
		 * because each write-back erases this info.
		 */
#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
		rx_desc->read.pkt_addr = cpu_to_le64(bi->dma);
#else
		rx_desc->read.pkt_addr = cpu_to_le64(dma);
#endif

		rx_desc++;
		bi++;
		i++;
		if (unlikely(!i)) {
			rx_desc = TXGBE_RX_DESC(rx_ring, 0);
			bi = rx_ring->rx_buffer_info;
			i -= rx_ring->count;
		}

		/* clear the length for the next_to_use descriptor */
		rx_desc->wb.upper.length = 0;

		count--;
	} while (count);

	i += rx_ring->count;

	if (rx_ring->next_to_use != i) {
		rx_ring->next_to_use = i;

#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
		/* update next to alloc since we have filled the ring */
		rx_ring->next_to_alloc = i;
#endif

		/* Force memory writes to complete before letting h/w
		 * know there are new descriptors to fetch.  (Only
		 * applicable for weak-ordered memory model archs,
		 * such as IA-64).
		 */
		wmb();
		writel(i, rx_ring->tail);
	}

	return ok;
}

#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
void txgbe_alloc_rx_buffers_zc(struct txgbe_ring *rx_ring, u16 count)
{
	__txgbe_alloc_rx_buffers_zc(rx_ring, count,
				    txgbe_alloc_buffer_slow_zc);
}

static bool txgbe_alloc_rx_buffers_fast_zc(struct txgbe_ring *rx_ring,
					   u16 count)
{
	return __txgbe_alloc_rx_buffers_zc(rx_ring, count,
					   txgbe_alloc_buffer_zc);
}
#endif /* HAVE_MEM_TYPE_XSK_BUFF_POOL */

static struct sk_buff *txgbe_construct_skb_zc(struct txgbe_ring *rx_ring,
#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
					      struct txgbe_rx_buffer *bi,
					      struct xdp_buff *xdp)
#else
					      struct txgbe_rx_buffer *bi)
#endif
{
#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
	struct xdp_buff *xdp_buffer = xdp;
#else
	struct xdp_buff *xdp_buffer = bi->xdp;
#endif
	unsigned int metasize = xdp_buffer->data - xdp_buffer->data_meta;
	unsigned int datasize = xdp_buffer->data_end - xdp_buffer->data;
	struct sk_buff *skb;

	/* allocate a skb to store the frags */
	skb = __napi_alloc_skb(&rx_ring->q_vector->napi,
			       xdp_buffer->data_end - xdp_buffer->data_hard_start,
			       GFP_ATOMIC | __GFP_NOWARN);
	if (unlikely(!skb))
		return NULL;

	skb_reserve(skb, xdp_buffer->data - xdp_buffer->data_hard_start);
	memcpy(__skb_put(skb, datasize), xdp_buffer->data, datasize);
	if (metasize)
		skb_metadata_set(skb, metasize);
#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
	txgbe_reuse_rx_buffer_zc(rx_ring, bi);
#else
	xsk_buff_free(xdp_buffer);
	bi->xdp = NULL;
#endif
	return skb;
}

static void txgbe_inc_ntc(struct txgbe_ring *rx_ring)
{
	u32 ntc = rx_ring->next_to_clean + 1;

	ntc = (ntc < rx_ring->count) ? ntc : 0;
	rx_ring->next_to_clean = ntc;
	prefetch(TXGBE_RX_DESC(rx_ring, ntc));
}

int txgbe_clean_rx_irq_zc(struct txgbe_q_vector *q_vector,
			  struct txgbe_ring *rx_ring,
			  const int budget)
{
	unsigned int total_rx_bytes = 0, total_rx_packets = 0;
	struct txgbe_adapter *adapter = q_vector->adapter;
	u16 cleaned_count = txgbe_desc_unused(rx_ring);
	unsigned int xdp_res, xdp_xmit = 0;
	bool failure = false;
	struct sk_buff *skb;
#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
	struct xdp_buff xdp;

	xdp.rxq = &rx_ring->xdp_rxq;
#endif
	

	while (likely(total_rx_packets < budget)) {
		union txgbe_rx_desc *rx_desc;
		struct txgbe_rx_buffer *bi;
		unsigned int size;

		/* return some buffers to hardware, one at a time is too slow */
		if (cleaned_count >= TXGBE_RX_BUFFER_WRITE) {
			failure = failure ||
#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
				  !txgbe_alloc_rx_buffers_fast_zc(rx_ring,
								  cleaned_count);
#else
				  !txgbe_alloc_rx_buffers_zc(rx_ring,
							     cleaned_count);
#endif
			cleaned_count = 0;
		}

		rx_desc = TXGBE_RX_DESC(rx_ring, rx_ring->next_to_clean);
		size = le16_to_cpu(rx_desc->wb.upper.length);
		if (!size)
			break;

		/* This memory barrier is needed to keep us from reading
		 * any other fields out of the rx_desc until we know the
		 * descriptor has been written back
		 */
		dma_rmb();

#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
		bi = txgbe_get_rx_buffer_zc(rx_ring, size);
#else
		bi = &rx_ring->rx_buffer_info[rx_ring->next_to_clean];
#endif

		if (unlikely(!txgbe_test_staterr(rx_desc,
						 TXGBE_RXD_STAT_EOP))) {
			struct txgbe_rx_buffer *next_bi;

#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
			txgbe_reuse_rx_buffer_zc(rx_ring, bi);
#else
			xsk_buff_free(bi->xdp);
			bi->xdp = NULL;
#endif
			txgbe_inc_ntc(rx_ring);
			next_bi =
			       &rx_ring->rx_buffer_info[rx_ring->next_to_clean];
#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
			next_bi->skb = ERR_PTR(-EINVAL);
#else
			next_bi->discard = true;
#endif
			continue;
		}

#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
		if (unlikely(bi->skb)) {
			txgbe_reuse_rx_buffer_zc(rx_ring, bi);
#else
		if (unlikely(bi->discard)) {
			xsk_buff_free(bi->xdp);
			bi->xdp = NULL;
			bi->discard = false;
#endif
			txgbe_inc_ntc(rx_ring);
			continue;
		}

#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
		xdp.data = bi->addr;
		xdp.data_meta = xdp.data;
		xdp.data_hard_start = xdp.data - XDP_PACKET_HEADROOM;
		xdp.data_end = xdp.data + size;
		xdp.handle = bi->handle;

		xdp_res = txgbe_run_xdp_zc(adapter, rx_ring, &xdp);
#else
		bi->xdp->data_end = bi->xdp->data + size;
		xsk_buff_dma_sync_for_cpu(bi->xdp, rx_ring->xsk_pool);
		xdp_res = txgbe_run_xdp_zc(adapter, rx_ring, bi->xdp);
#endif

		if (xdp_res) {
			if (xdp_res & (TXGBE_XDP_TX | TXGBE_XDP_REDIR)) {
				xdp_xmit |= xdp_res;
#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
				bi->addr = NULL;
				bi->skb = NULL;
#endif
			} else {
#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
				txgbe_reuse_rx_buffer_zc(rx_ring, bi);
#else
				xsk_buff_free(bi->xdp);
#endif
			}
#ifdef HAVE_MEM_TYPE_XSK_BUFF_POOL
			bi->xdp = NULL;
#endif
			total_rx_packets++;
			total_rx_bytes += size;

			cleaned_count++;
			txgbe_inc_ntc(rx_ring);
			continue;
		}

		/* XDP_PASS path */
#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
		skb = txgbe_construct_skb_zc(rx_ring, bi, &xdp);
#else
		skb = txgbe_construct_skb_zc(rx_ring, bi);
#endif
		if (!skb) {
			rx_ring->rx_stats.alloc_rx_buff_failed++;
			break;
		}

		cleaned_count++;
		txgbe_inc_ntc(rx_ring);

		if (eth_skb_pad(skb))
			continue;

		total_rx_bytes += skb->len;
		total_rx_packets++;

		txgbe_process_skb_fields(rx_ring, rx_desc, skb);
		txgbe_rx_skb(q_vector, rx_ring, rx_desc, skb);
	}

	if (xdp_xmit & TXGBE_XDP_REDIR)
		xdp_do_flush_map();

	if (xdp_xmit & TXGBE_XDP_TX) {
		struct txgbe_ring *ring = adapter->xdp_ring[smp_processor_id() % adapter->num_xdp_queues];
		if (static_branch_unlikely(&txgbe_xdp_locking_key))
			spin_lock(&ring->tx_lock);
		wmb();
		writel(ring->next_to_use, ring->tail);
		if (static_branch_unlikely(&txgbe_xdp_locking_key))
			spin_unlock(&ring->tx_lock);
	}

	u64_stats_update_begin(&rx_ring->syncp);
	rx_ring->stats.packets += total_rx_packets;
	rx_ring->stats.bytes += total_rx_bytes;
	u64_stats_update_end(&rx_ring->syncp);
	q_vector->rx.total_packets += total_rx_packets;
	q_vector->rx.total_bytes += total_rx_bytes;

	return failure ? budget : (int)total_rx_packets;
}

#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
void txgbe_xsk_clean_rx_ring(struct txgbe_ring *rx_ring)
{
	u16 i = rx_ring->next_to_clean;
	struct txgbe_rx_buffer *bi = &rx_ring->rx_buffer_info[i];

	while (i != rx_ring->next_to_alloc) {
		xsk_umem_fq_reuse(rx_ring->xsk_pool, bi->handle);
		i++;
		bi++;
		if (i == rx_ring->count) {
			i = 0;
			bi = rx_ring->rx_buffer_info;
		}
	}
}
#else
void txgbe_xsk_clean_rx_ring(struct txgbe_ring *rx_ring)
{
	struct txgbe_rx_buffer *bi;
	u16 i;

	for (i = 0; i < rx_ring->count; i++) {
		bi = &rx_ring->rx_buffer_info[i];

		if (!bi->xdp)
			continue;

		xsk_buff_free(bi->xdp);
		bi->xdp = NULL;
	}
}
#endif

static bool txgbe_xmit_zc(struct txgbe_ring *xdp_ring, unsigned int budget)
{
	union txgbe_tx_desc *tx_desc = NULL;
	u16 ntu = xdp_ring->next_to_use;
	struct txgbe_tx_buffer *tx_bi;
	bool work_done = true;
#ifdef XSK_UMEM_RETURNS_XDP_DESC
	struct xdp_desc desc;
#endif
	dma_addr_t dma;
	u32 cmd_type;
#ifndef XSK_UMEM_RETURNS_XDP_DESC
	u32 len;
#endif
	while (budget-- > 0) {
		if (unlikely(!txgbe_desc_unused(xdp_ring))) {
			work_done = false;
			break;
		}

		if (!netif_carrier_ok(xdp_ring->netdev))
			break;

#ifdef XSK_UMEM_RETURNS_XDP_DESC
		if (!xsk_tx_peek_desc(xdp_ring->xsk_pool, &desc))
			break;
#ifndef HAVE_MEM_TYPE_XSK_BUFF_POOL
		dma = xdp_umem_get_dma(xdp_ring->xsk_pool, desc.addr);
		dma_sync_single_for_device(xdp_ring->dev, dma, desc.len,
					   DMA_BIDIRECTIONAL);
#else
		dma = xsk_buff_raw_get_dma(xdp_ring->xsk_pool, desc.addr);
		xsk_buff_raw_dma_sync_for_device(xdp_ring->xsk_pool, dma,
						 desc.len);
#endif

		tx_bi = &xdp_ring->tx_buffer_info[ntu];
		tx_bi->bytecount = desc.len;
		tx_bi->gso_segs = 1;
		tx_bi->xdpf = NULL;
#else
		if (!xsk_umem_consume_tx(xdp_ring->xsk_pool, &dma, &len))
			break;

		dma_sync_single_for_device(xdp_ring->dev, dma, len,
					   DMA_BIDIRECTIONAL);

		tx_bi = &xdp_ring->tx_buffer_info[ntu];
		tx_bi->bytecount = len;
		tx_bi->gso_segs = 1;
		tx_bi->xdpf = NULL;

#endif
		tx_desc = TXGBE_TX_DESC(xdp_ring, ntu);
		tx_desc->read.olinfo_status = 0;
		tx_desc->read.buffer_addr = cpu_to_le64(dma);

		/* put descriptor type bits */
		cmd_type = txgbe_tx_cmd_type(tx_bi->tx_flags);
#ifdef XSK_UMEM_RETURNS_XDP_DESC
		cmd_type |= desc.len | TXGBE_TXD_CMD;
		tx_desc->read.cmd_type_len = cpu_to_le32(cmd_type);
		tx_desc->read.olinfo_status =
			cpu_to_le32(desc.len << TXGBE_TXD_PAYLEN_SHIFT);
#else
		cmd_type |= len | TXGBE_TXD_CMD;
		tx_desc->read.cmd_type_len = cpu_to_le32(cmd_type);
		tx_desc->read.olinfo_status =
			cpu_to_le32(len << TXGBE_TXD_PAYLEN_SHIFT);
#endif
		smp_wmb();
		tx_bi->next_to_watch = tx_desc;
		ntu++;
		if (ntu == xdp_ring->count)
			ntu = 0;
		xdp_ring->next_to_use = ntu;

	}
	if (tx_desc) {
		wmb();
		writel(xdp_ring->next_to_use, xdp_ring->tail);
		xsk_tx_release(xdp_ring->xsk_pool);
	}

	return (budget > 0) && work_done;
}

static void txgbe_clean_xdp_tx_buffer(struct txgbe_ring *tx_ring,
				      struct txgbe_tx_buffer *tx_bi)
{
	xdp_return_frame(tx_bi->xdpf);
	tx_ring->xdp_tx_active--;
	dma_unmap_single(tx_ring->dev,
			 dma_unmap_addr(tx_bi, dma),
			 dma_unmap_len(tx_bi, len), DMA_TO_DEVICE);
	dma_unmap_len_set(tx_bi, len, 0);
}

bool txgbe_clean_xdp_tx_irq(struct txgbe_q_vector *q_vector,
			    struct txgbe_ring *tx_ring)
{
	u32 ntu = tx_ring->next_to_use, ntc = tx_ring->next_to_clean;
	union txgbe_tx_desc *tx_desc;
	struct txgbe_tx_buffer *tx_bi;
	unsigned int total_packets = 0, total_bytes = 0;
	u32 xsk_frames = 0;

	tx_bi = &tx_ring->tx_buffer_info[ntc];
	tx_desc = TXGBE_TX_DESC(tx_ring, ntc);
	while (ntc != ntu) {
		if (!(tx_desc->wb.status & cpu_to_le32(TXGBE_TXD_STAT_DD)))
			break;

		total_bytes += tx_bi->bytecount;
		total_packets += tx_bi->gso_segs;

		if (tx_bi->xdpf)
			txgbe_clean_xdp_tx_buffer(tx_ring, tx_bi);
		else
			xsk_frames++;

		tx_bi->xdpf = NULL;

		tx_bi++;
		tx_desc++;
		ntc++;
		if (unlikely(ntc == tx_ring->count)) {
			ntc = 0;
			tx_bi = tx_ring->tx_buffer_info;
			tx_desc = TXGBE_TX_DESC(tx_ring, 0);
		}

		/* issue prefetch for next Tx descriptor */
		prefetch(tx_desc);
	}
	tx_ring->next_to_clean = ntc;

	if (unlikely(tx_ring->next_to_clean >= tx_ring->count))
		tx_ring->next_to_clean -= tx_ring->count;

	u64_stats_update_begin(&tx_ring->syncp);
	tx_ring->stats.bytes += total_bytes;
	tx_ring->stats.packets += total_packets;
	u64_stats_update_end(&tx_ring->syncp);
	tx_ring->q_vector->tx.total_bytes += total_bytes;
	tx_ring->q_vector->tx.total_packets += total_packets;

	if (xsk_frames) {
		xsk_tx_completed(tx_ring->xsk_pool, xsk_frames);
	}
	return txgbe_xmit_zc(tx_ring, q_vector->tx.work_limit);
}

#ifdef HAVE_NDO_XSK_WAKEUP
int txgbe_xsk_wakeup(struct net_device *dev, u32 qid, u32 __maybe_unused flags)
#else
int txgbe_xsk_async_xmit(struct net_device *dev, u32 qid)
#endif
{
	struct txgbe_adapter *adapter = netdev_priv(dev);
	struct txgbe_ring *ring;
	
	if (test_bit(__TXGBE_DOWN, &adapter->state))
		return -ENETDOWN;
	if (!READ_ONCE(adapter->xdp_prog))
		return -ENXIO;

	if (qid >= adapter->num_xdp_queues)
		return -ENXIO;

	if (!adapter->xsk_pools || !adapter->xsk_pools[qid])
		return -ENXIO;

	ring = adapter->xdp_ring[qid];
	if (!napi_if_scheduled_mark_missed(&ring->q_vector->napi)) {
		if (likely(napi_schedule_prep(&ring->q_vector->napi)))
			__napi_schedule(&ring->q_vector->napi);
	}

	return 0;
}

void txgbe_xsk_clean_tx_ring(struct txgbe_ring *tx_ring)
{
	u16 ntc = tx_ring->next_to_clean, ntu = tx_ring->next_to_use;
	struct txgbe_tx_buffer *tx_bi;
	u32 xsk_frames = 0;

	while (ntc != ntu) {
		tx_bi = &tx_ring->tx_buffer_info[ntc];

		if (tx_bi->xdpf)
			txgbe_clean_xdp_tx_buffer(tx_ring, tx_bi);
		else
			xsk_frames++;

		tx_bi->xdpf = NULL;

		ntc++;
		if (ntc == tx_ring->count)
			ntc = 0;
	}

	if (xsk_frames)
		xsk_tx_completed(tx_ring->xsk_pool, xsk_frames);
}
#endif /* HAVE_AF_XDP_ZC_SUPPORT */
