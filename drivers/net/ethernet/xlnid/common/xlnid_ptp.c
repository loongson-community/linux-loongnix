// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2008 - 2022 Xel Technology. */

#include "xlnid.h"
#include <linux/ptp_classify.h>

#define XLNID_INCVAL_10GB 0x66666666
#define XLNID_INCVAL_1GB  0x40000000
#define XLNID_INCVAL_100  0x50000000

#define XLNID_INCVAL_SHIFT_10GB  28
#define XLNID_INCVAL_SHIFT_1GB   24
#define XLNID_INCVAL_SHIFT_100   21

#define XLNID_INCVAL_SHIFT       7
#define XLNID_INCPER_SHIFT       24

#define XLNID_OVERFLOW_PERIOD    (HZ * 30)
#define XLNID_PTP_TX_TIMEOUT     (HZ)

/* half of a one second clock period, for use with PPS signal. We have to use
 * this instead of something pre-defined like XLNID_PTP_PPS_HALF_SECOND, in
 * order to force at least 64bits of precision for shifting
 */
#define XLNID_PTP_PPS_HALF_SECOND 500000000ULL

#define INCVALUE_MASK	0x7FFFFFFF
#define ISGN		0x80000000
#define MAX_TIMADJ	0x7FFFFFFF

/**
 * xlnid_ptp_read_skylake - read raw cycle counter (to be used by time counter)
 * @hw_cc: the cyclecounter structure
 *
 * this function reads the cyclecounter registers and is called by the
 * cyclecounter structure used to construct a ns counter from the
 * arbitrary fixed point registers
 */
static u64 xlnid_ptp_read_skylake(const struct cyclecounter *hw_cc)
{
#if 0
	struct xlnid_adapter *adapter =
		container_of(hw_cc, struct xlnid_adapter, hw_cc);
	struct xlnid_hw *hw = &adapter->hw;
	u64 stamp = 0;

	stamp |= (u64)XLNID_READ_REG(hw, XLNID_SYSTIML);
	stamp |= (u64)XLNID_READ_REG(hw, XLNID_SYSTIMH) << 32;

	return stamp;
#endif
    return 0;
}

/**
 * xlnid_ptp_convert_to_hwtstamp - convert register value to hw timestamp
 * @adapter: private adapter structure
 * @hwtstamp: stack timestamp structure
 * @timestamp: unsigned 64bit system time value
 *
 * We need to convert the adapter's RX/TXSTMP registers into a hwtstamp value
 * which can be used by the stack's ptp functions.
 *
 * The lock is used to protect consistency of the cyclecounter and the SYSTIME
 * registers. However, it does not need to protect against the Rx or Tx
 * timestamp registers, as there can't be a new timestamp until the old one is
 * unlatched by reading.
 *
 * In addition to the timestamp in hardware, some controllers need a software
 * overflow cyclecounter, and this function takes this into account as well.
 **/
static void xlnid_ptp_convert_to_hwtstamp(struct xlnid_adapter *adapter,
					  struct skb_shared_hwtstamps *hwtstamp,
					  u64 timestamp)
{
	unsigned long flags;
//	struct timespec64 systime;
	u64 ns;

	memset(hwtstamp, 0, sizeof(*hwtstamp));

	spin_lock_irqsave(&adapter->tmreg_lock, flags);
	ns = timecounter_cyc2time(&adapter->hw_tc, timestamp);
	spin_unlock_irqrestore(&adapter->tmreg_lock, flags);

	hwtstamp->hwtstamp = ns_to_ktime(ns);
}

/**
 * xlnid_ptp_adjfreq_skylake
 * @ptp: the ptp clock structure
 * @ppb: parts per billion adjustment from base
 *
 * adjust the frequency of the ptp cycle counter by the
 * indicated ppb from the base frequency.
 */
static int xlnid_ptp_adjfreq_skylake(struct ptp_clock_info *ptp, s32 ppb)
{
	struct xlnid_adapter *adapter =
		container_of(ptp, struct xlnid_adapter, ptp_caps);
//	struct xlnid_hw *hw = &adapter->hw;
	u64 freq, incval;
	u32 diff;
	int neg_adj = 0;

	if (ppb < 0) {
		neg_adj = 1;
		ppb = -ppb;
	}

	smp_mb();
	incval = READ_ONCE(adapter->base_incval);

	freq = incval;
	freq *= ppb;
	diff = div_u64(freq, 1000000000ULL);

	incval = neg_adj ? (incval - diff) : (incval + diff);
    if (incval > 0x00FFFFFFULL)
		e_dev_warn("PTP ppb adjusted SYSTIME rate overflowed!\n");
	//XLNID_WRITE_REG(hw, XLNID_TIMINCA,
			//(1 << XLNID_INCPER_SHIFT) |
			//((u32)incval & 0x00FFFFFFUL));

	return 0;
}



/**
 * xlnid_ptp_adjtime_timecounter
 * @ptp: the ptp clock structure
 * @delta: offset to adjust the cycle counter by
 *
 * adjust the timer by resetting the timecounter structure.
 */
static int xlnid_ptp_adjtime_timecounter(struct ptp_clock_info *ptp,
					 s64 delta)
{
	struct xlnid_adapter *adapter =
		container_of(ptp, struct xlnid_adapter, ptp_caps);
	unsigned long flags;

	spin_lock_irqsave(&adapter->tmreg_lock, flags);
	timecounter_adjtime(&adapter->hw_tc, delta);
	spin_unlock_irqrestore(&adapter->tmreg_lock, flags);

	if (adapter->ptp_setup_sdp)
		adapter->ptp_setup_sdp(adapter);

	return 0;
}

/**
 * xlnid_ptp_gettime64_timecounter
 * @ptp: the ptp clock structure
 * @ts: timespec64 structure to hold the current time value
 *
 * read the timecounter and return the correct value on ns,
 * after converting it into a struct timespec64.
 */
static int xlnid_ptp_gettime64_timecounter(struct ptp_clock_info *ptp, struct timespec64 *ts)
{
	struct xlnid_adapter *adapter =
		container_of(ptp, struct xlnid_adapter, ptp_caps);
	unsigned long flags;
	u64 ns;

	spin_lock_irqsave(&adapter->tmreg_lock, flags);
	ns = timecounter_read(&adapter->hw_tc);
	spin_unlock_irqrestore(&adapter->tmreg_lock, flags);

	*ts = ns_to_timespec64(ns);

	return 0;
}

/**
 * xlnid_ptp_settime64_timecounter
 * @ptp: the ptp clock structure
 * @ts: the timespec64 containing the new time for the cycle counter
 *
 * reset the timecounter to use a new base value instead of the kernel
 * wall timer value.
 */
static int xlnid_ptp_settime64_timecounter(struct ptp_clock_info *ptp,
					   const struct timespec64 *ts)
{
	struct xlnid_adapter *adapter =
		container_of(ptp, struct xlnid_adapter, ptp_caps);
	u64 ns;
	unsigned long flags;

	ns = timespec64_to_ns(ts);

	/* reset the timecounter */
	spin_lock_irqsave(&adapter->tmreg_lock, flags);
	timecounter_init(&adapter->hw_tc, &adapter->hw_cc, ns);
	spin_unlock_irqrestore(&adapter->tmreg_lock, flags);

	if (adapter->ptp_setup_sdp)
		adapter->ptp_setup_sdp(adapter);
	return 0;
}

#ifndef HAVE_PTP_CLOCK_INFO_GETTIME64
static int xlnid_ptp_gettime_timecounter(struct ptp_clock_info *ptp, struct timespec *ts)
{
	struct timespec64 ts64;
	int err;

	err = xlnid_ptp_gettime64_timecounter(ptp, &ts64);
	if (err)
		return err;

	*ts = timespec64_to_timespec(ts64);

	return 0;
}

static int xlnid_ptp_settime_timecounter(struct ptp_clock_info *ptp,
					 const struct timespec *ts)
{
	struct timespec64 ts64;

	ts64 = timespec_to_timespec64(*ts);
	return xlnid_ptp_settime64_timecounter(ptp, &ts64);
}
#endif

/**
 * xlnid_ptp_feature_enable
 * @ptp: the ptp clock structure
 * @rq: the requested feature to change
 * @on: whether to enable or disable the feature
 *
 * enable (or disable) ancillary features of the phc subsystem.
 */
static int xlnid_ptp_feature_enable(struct ptp_clock_info *ptp,
				    struct ptp_clock_request *rq, int on)
{
	struct xlnid_adapter *adapter =
		container_of(ptp, struct xlnid_adapter, ptp_caps);

	/**
	 * When PPS is enabled, unmask the interrupt for the ClockOut
	 * feature, so that the interrupt handler can send the PPS
	 * event when the clock SDP triggers. Clear mask when PPS is
	 * disabled
	 */
	if (rq->type == PTP_CLK_REQ_PPS && adapter->ptp_setup_sdp) {
		if (on)
			adapter->flags2 |= XLNID_FLAG2_PTP_PPS_ENABLED;
		else
			adapter->flags2 &= ~XLNID_FLAG2_PTP_PPS_ENABLED;

		adapter->ptp_setup_sdp(adapter);
		return 0;
	}

	return -ENOTSUPP;
}

/**
 * xlnid_ptp_check_pps_event
 * @adapter: the private adapter structure
 *
 * This function is called by the interrupt routine when checking for
 * interrupts. It will check and handle a pps event.
 */
void xlnid_ptp_check_pps_event(struct xlnid_adapter *adapter)
{
#if 0
	struct xlnid_hw *hw = &adapter->hw;
	struct ptp_clock_event event;

	event.type = PTP_CLOCK_PPS;

	/* this check is necessary in case the interrupt was enabled via some
	 * alternative means (ex. debug_fs). Better to check here than
	 * everywhere that calls this function.
	 */
	if (!adapter->ptp_clock)
		return;
#endif

	return;
}

/**
 * xlnid_ptp_overflow_check - watchdog task to detect SYSTIME overflow
 * @adapter: private adapter struct
 *
 * this watchdog task periodically reads the timecounter
 * in order to prevent missing when the system time registers wrap
 * around. This needs to be run approximately twice a minute for the fastest
 * overflowing hardware. We run it for all hardware since it shouldn't have a
 * large impact.
 */
void xlnid_ptp_overflow_check(struct xlnid_adapter *adapter)
{
	bool timeout = time_is_before_jiffies(adapter->last_overflow_check +
					      XLNID_OVERFLOW_PERIOD);
	struct timespec64 ts;

	if (timeout) {
		xlnid_ptp_gettime64_timecounter(&adapter->ptp_caps, &ts);
		adapter->last_overflow_check = jiffies;
	}
}

/**
 * xlnid_ptp_rx_hang - detect error case when Rx timestamp registers latched
 * @adapter: private network adapter structure
 *
 * this watchdog task is scheduled to detect error case where hardware has
 * dropped an Rx packet that was timestamped when the ring is full. The
 * particular error is rare but leaves the device in a state unable to timestamp
 * any future packets.
 */
void xlnid_ptp_rx_hang(struct xlnid_adapter *adapter)
{
//	struct xlnid_hw *hw = &adapter->hw;
	struct xlnid_ring *rx_ring;
//	u32 tsyncrxctl = XLNID_READ_REG(hw, XLNID_TSYNCRXCTL);
	unsigned long rx_event;
	int n;

#if 0
	/* if we don't have a valid timestamp in the registers, just update the
	 * timeout counter and exit
	 */
	if (!(tsyncrxctl & XLNID_TSYNCRXCTL_VALID)) {
		adapter->last_rx_ptp_check = jiffies;
		return;
	}
#endif

	/* determine the most recent watchdog or rx_timestamp event */
	rx_event = adapter->last_rx_ptp_check;
	for (n = 0; n < adapter->num_rx_queues; n++) {
		rx_ring = adapter->rx_ring[n];
		if (time_after(rx_ring->last_rx_timestamp, rx_event))
			rx_event = rx_ring->last_rx_timestamp;
	}

	/* only need to read the high RXSTMP register to clear the lock */
	if (time_is_before_jiffies(rx_event + 5*HZ)) {
//		XLNID_READ_REG(hw, XLNID_RXSTMPH);
		adapter->last_rx_ptp_check = jiffies;

		adapter->rx_hwtstamp_cleared++;
		e_warn(drv, "clearing RX Timestamp hang");
	}
}

/**
 * xlnid_ptp_clear_tx_timestamp - utility function to clear Tx timestamp state
 * @adapter: the private adapter structure
 *
 * This function should be called whenever the state related to a Tx timestamp
 * needs to be cleared. This helps ensure that all related bits are reset for
 * the next Tx timestamp event.
 */
static void xlnid_ptp_clear_tx_timestamp(struct xlnid_adapter *adapter)
{
//	struct xlnid_hw *hw = &adapter->hw;

//	XLNID_READ_REG(hw, XLNID_TXSTMPH);
	if (adapter->ptp_tx_skb) {
		dev_kfree_skb_any(adapter->ptp_tx_skb);
		adapter->ptp_tx_skb = NULL;
	}
	clear_bit_unlock(__XLNID_PTP_TX_IN_PROGRESS, &adapter->state);
}

/**
 * xlnid_ptp_tx_hang - detect error case where Tx timestamp never finishes
 * @adapter: private network adapter structure
 */
void xlnid_ptp_tx_hang(struct xlnid_adapter *adapter)
{
	bool timeout = time_is_before_jiffies(adapter->ptp_tx_start +
					      XLNID_PTP_TX_TIMEOUT);

	if (!adapter->ptp_tx_skb)
		return;

	if (!test_bit(__XLNID_PTP_TX_IN_PROGRESS, &adapter->state))
		return;

	/* If we haven't received a timestamp within the timeout, it is
	 * reasonable to assume that it will never occur, so we can unlock the
	 * timestamp bit when this occurs.
	 */
	if (timeout) {
		cancel_work_sync(&adapter->ptp_tx_work);
		xlnid_ptp_clear_tx_timestamp(adapter);
		adapter->tx_hwtstamp_timeouts++;
		e_warn(drv, "clearing Tx timestamp hang\n");
	}
}

/**
 * xlnid_ptp_tx_hwtstamp - utility function which checks for TX time stamp
 * @adapter: the private adapter struct
 *
 * if the timestamp is valid, we convert it into the timecounter ns
 * value, then store that result into the shhwtstamps structure which
 * is passed up the network stack
 */
 #if 0
static void xlnid_ptp_tx_hwtstamp(struct xlnid_adapter *adapter)
{
	struct sk_buff *skb = adapter->ptp_tx_skb;
	struct xlnid_hw *hw = &adapter->hw;
	struct skb_shared_hwtstamps shhwtstamps;
	u64 regval = 0;

	regval |= (u64)XLNID_READ_REG(hw, XLNID_TXSTMPL);
	regval |= (u64)XLNID_READ_REG(hw, XLNID_TXSTMPH) << 32;
	xlnid_ptp_convert_to_hwtstamp(adapter, &shhwtstamps, regval);

	/* Handle cleanup of the ptp_tx_skb ourselves, and unlock the state
	 * bit prior to notifying the stack via skb_tstamp_tx(). This prevents
	 * well behaved applications from attempting to timestamp again prior
	 * to the lock bit being clear.
	 */
	adapter->ptp_tx_skb = NULL;
	clear_bit_unlock(__XLNID_PTP_TX_IN_PROGRESS, &adapter->state);

	/* Notify the stack and then free the skb after we've unlocked */
	skb_tstamp_tx(skb, &shhwtstamps);
	dev_kfree_skb_any(skb);
}
#endif

/**
 * xlnid_ptp_tx_hwtstamp_work
 * @work: pointer to the work struct
 *
 * This work item polls TSYNCTXCTL valid bit to determine when a Tx hardware
 * timestamp has been taken for the current skb. It is necesary, because the
 * descriptor's "done" bit does not correlate with the timestamp event.
 */
static void xlnid_ptp_tx_hwtstamp_work(struct work_struct *work)
{
	struct xlnid_adapter *adapter = container_of(work, struct xlnid_adapter,
						     ptp_tx_work);
//	struct xlnid_hw *hw = &adapter->hw;
	bool timeout = time_is_before_jiffies(adapter->ptp_tx_start +
					      XLNID_PTP_TX_TIMEOUT);
//	u32 tsynctxctl;

	/* we have to have a valid skb to poll for a timestamp */
	if (!adapter->ptp_tx_skb) {
		xlnid_ptp_clear_tx_timestamp(adapter);
		return;
	}

#if 0
	/* stop polling once we have a valid timestamp */
	tsynctxctl = XLNID_READ_REG(hw, XLNID_TSYNCTXCTL);
	if (tsynctxctl & XLNID_TSYNCTXCTL_VALID) {
		xlnid_ptp_tx_hwtstamp(adapter);
		return;
	}
#endif

	/* check timeout last in case timestamp event just occurred */
	if (timeout) {
		xlnid_ptp_clear_tx_timestamp(adapter);
		adapter->tx_hwtstamp_timeouts++;
		e_warn(drv, "clearing Tx Timestamp hang");
	} else {
		/* reschedule to keep checking until we timeout */
		schedule_work(&adapter->ptp_tx_work);
	}
}

/**
 * xlnid_ptp_rx_pktstamp - utility function to get RX time stamp from buffer
 * @q_vector: structure containing interrupt and ring information
 * @skb: the packet
 *
 * This function will be called by the Rx routine of the timestamp for this
 * packet is stored in the buffer. The value is stored in little endian format
 * starting at the end of the packet data.
 */
void xlnid_ptp_rx_pktstamp(struct xlnid_q_vector *q_vector,
			   struct sk_buff *skb)
{
	__le64 regval;

	/* copy the bits out of the skb, and then trim the skb length */
	skb_copy_bits(skb, skb->len - XLNID_TS_HDR_LEN, &regval, XLNID_TS_HDR_LEN);
	__pskb_trim(skb, skb->len - XLNID_TS_HDR_LEN);

	/* The timestamp is recorded in little endian format, and is stored at
	 * the end of the packet.
	 *
	 * DWORD: N              N + 1      N + 2
	 * Field: End of Packet  SYSTIMH    SYSTIML
	 */
	xlnid_ptp_convert_to_hwtstamp(q_vector->adapter, skb_hwtstamps(skb),
				      le64_to_cpu(regval));
}

/**
 * xlnid_ptp_rx_rgtstamp - utility function which checks for RX time stamp
 * @q_vector: structure containing interrupt and ring information
 * @skb: particular skb to send timestamp with
 *
 * if the timestamp is valid, we convert it into the timecounter ns
 * value, then store that result into the shhwtstamps structure which
 * is passed up the network stack
 */
void xlnid_ptp_rx_rgtstamp(struct xlnid_q_vector *q_vector,
			   struct sk_buff *skb)
{
	struct xlnid_adapter *adapter;
	struct xlnid_hw *hw;
	u64 regval = 0;
//	u32 tsyncrxctl;

	/* we cannot process timestamps on a ring without a q_vector */
	if (!q_vector || !q_vector->adapter)
		return;

	adapter = q_vector->adapter;
	hw = &adapter->hw;

#if 0
	/*
	 * Read the tsyncrxctl register afterwards in order to prevent taking an
	 * I/O hit on every packet.
	 */
	tsyncrxctl = XLNID_READ_REG(hw, XLNID_TSYNCRXCTL);
	if (!(tsyncrxctl & XLNID_TSYNCRXCTL_VALID))
		return;

	regval |= (u64)XLNID_READ_REG(hw, XLNID_RXSTMPL);
	regval |= (u64)XLNID_READ_REG(hw, XLNID_RXSTMPH) << 32;
#endif
	xlnid_ptp_convert_to_hwtstamp(adapter, skb_hwtstamps(skb), regval);
}

/**
 * xlnid_ptp_get_ts_config - get current hardware timestamping configuration
 * @adapter: pointer to adapter structure
 * @ifr: ioctl data
 *
 * This function returns the current timestamping settings. Rather than
 * attempt to deconstruct registers to fill in the values, simply keep a copy
 * of the old settings around, and return a copy when requested.
 */
int xlnid_ptp_get_ts_config(struct xlnid_adapter *adapter, struct ifreq *ifr)
{
	struct hwtstamp_config *config = &adapter->tstamp_config;

	return copy_to_user(ifr->ifr_data, config, sizeof(*config)) ?
		-EFAULT : 0;
}

/**
 * xlnid_ptp_set_timestamp_mode - setup the hardware for the requested mode
 * @adapter: the private xlnid adapter structure
 * @config: the hwtstamp configuration requested
 *
 * Outgoing time stamping can be enabled and disabled. Play nice and
 * disable it when requested, although it shouldn't cause any overhead
 * when no packet needs it. At most one packet in the queue may be
 * marked for time stamping, otherwise it would be impossible to tell
 * for sure to which packet the hardware time stamp belongs.
 *
 * Incoming time stamping has to be configured via the hardware
 * filters. Not all combinations are supported, in particular event
 * type has to be specified. Matching the kind of event packet is
 * not supported, with the exception of "all V2 events regardless of
 * level 2 or 4".
 *
 * Since hardware always timestamps Path delay packets when timestamping V2
 * packets, regardless of the type specified in the register, only use V2
 * Event mode. This more accurately tells the user what the hardware is going
 * to do anyways.
 *
 * Note: this may modify the hwtstamp configuration towards a more general
 * mode, if required to support the specifically requested mode.
 */
static int xlnid_ptp_set_timestamp_mode(struct xlnid_adapter *adapter,
					struct hwtstamp_config *config)
{
	struct xlnid_hw *hw = &adapter->hw;
	u32 tsync_tx_ctl = XLNID_TSYNCTXCTL_ENABLED;
	u32 tsync_rx_ctl = XLNID_TSYNCRXCTL_ENABLED;
	u32 tsync_rx_mtrl = PTP_EV_PORT << 16;
	bool is_l2 = false;
//	u32 regval;

	/* reserved for future extensions */
	if (config->flags)
		return -EINVAL;

	switch (config->tx_type) {
	case HWTSTAMP_TX_OFF:
		tsync_tx_ctl = 0;
	case HWTSTAMP_TX_ON:
		break;
	default:
		return -ERANGE;
	}

	switch (config->rx_filter) {
	case HWTSTAMP_FILTER_NONE:
		tsync_rx_ctl = 0;
		tsync_rx_mtrl = 0;
		adapter->flags &= ~(XLNID_FLAG_RX_HWTSTAMP_ENABLED |
				    XLNID_FLAG_RX_HWTSTAMP_IN_REGISTER);
		break;
	case HWTSTAMP_FILTER_PTP_V1_L4_SYNC:
		tsync_rx_ctl |= XLNID_TSYNCRXCTL_TYPE_L4_V1;
		tsync_rx_mtrl |= XLNID_RXMTRL_V1_SYNC_MSG;
		adapter->flags |= (XLNID_FLAG_RX_HWTSTAMP_ENABLED |
				   XLNID_FLAG_RX_HWTSTAMP_IN_REGISTER);
		break;
	case HWTSTAMP_FILTER_PTP_V1_L4_DELAY_REQ:
		tsync_rx_ctl |= XLNID_TSYNCRXCTL_TYPE_L4_V1;
		tsync_rx_mtrl |= XLNID_RXMTRL_V1_DELAY_REQ_MSG;
		adapter->flags |= (XLNID_FLAG_RX_HWTSTAMP_ENABLED |
				   XLNID_FLAG_RX_HWTSTAMP_IN_REGISTER);
		break;
	case HWTSTAMP_FILTER_PTP_V2_EVENT:
	case HWTSTAMP_FILTER_PTP_V2_L2_EVENT:
	case HWTSTAMP_FILTER_PTP_V2_L4_EVENT:
	case HWTSTAMP_FILTER_PTP_V2_SYNC:
	case HWTSTAMP_FILTER_PTP_V2_L2_SYNC:
	case HWTSTAMP_FILTER_PTP_V2_L4_SYNC:
	case HWTSTAMP_FILTER_PTP_V2_DELAY_REQ:
	case HWTSTAMP_FILTER_PTP_V2_L2_DELAY_REQ:
	case HWTSTAMP_FILTER_PTP_V2_L4_DELAY_REQ:
		tsync_rx_ctl |= XLNID_TSYNCRXCTL_TYPE_EVENT_V2;
		is_l2 = true;
		config->rx_filter = HWTSTAMP_FILTER_PTP_V2_EVENT;
		adapter->flags |= (XLNID_FLAG_RX_HWTSTAMP_ENABLED |
				   XLNID_FLAG_RX_HWTSTAMP_IN_REGISTER);
		break;
	case HWTSTAMP_FILTER_PTP_V1_L4_EVENT:
#ifdef HAVE_HWTSTAMP_FILTER_NTP_ALL
	case HWTSTAMP_FILTER_NTP_ALL:
#endif /* HAVE_HWTSTAMP_FILTER_NTP_ALL */
	case HWTSTAMP_FILTER_ALL:
		/* fall through */
	default:
		/* register RXMTRL must be set in order to do V1 packets,
		 * therefore it is not possible to time stamp both V1 Sync and
		 * Delay_Req messages unless hardware supports timestamping all
		 * packets => return error
		 */
		adapter->flags &= ~(XLNID_FLAG_RX_HWTSTAMP_ENABLED |
				    XLNID_FLAG_RX_HWTSTAMP_IN_REGISTER);
		config->rx_filter = HWTSTAMP_FILTER_NONE;
		return -ERANGE;
	}

	/* define ethertype filter for timestamping L2 packets */
	if (is_l2)
		XLNID_WRITE_REG(hw, ETQF(XLNID_ETQF_FILTER_1588),
				(XLNID_ETQF_FILTER_EN | /* enable filter */
				 XLNID_ETQF_1588 | /* enable timestamping */
				 ETH_P_1588));     /* 1588 eth protocol type */
	else
		XLNID_WRITE_REG(hw, ETQF(XLNID_ETQF_FILTER_1588), 0);

#if 0
	/* enable/disable TX */
	regval = XLNID_READ_REG(hw, XLNID_TSYNCTXCTL);
	regval &= ~XLNID_TSYNCTXCTL_ENABLED;
	regval |= tsync_tx_ctl;
	XLNID_WRITE_REG(hw, XLNID_TSYNCTXCTL, regval);

	/* enable/disable RX */
	regval = XLNID_READ_REG(hw, XLNID_TSYNCRXCTL);
	regval &= ~(XLNID_TSYNCRXCTL_ENABLED | XLNID_TSYNCRXCTL_TYPE_MASK);
	regval |= tsync_rx_ctl;
	XLNID_WRITE_REG(hw, XLNID_TSYNCRXCTL, regval);

	/* define which PTP packets are time stamped */
	XLNID_WRITE_REG(hw, XLNID_RXMTRL, tsync_rx_mtrl);

	XLNID_WRITE_FLUSH(hw);

	/* clear TX/RX timestamp state, just to be sure */
	xlnid_ptp_clear_tx_timestamp(adapter);
	XLNID_READ_REG(hw, XLNID_RXSTMPH);
#endif

	return 0;
}

/**
 * xlnid_ptp_set_ts_config - user entry point for timestamp mode
 * @adapter: pointer to adapter struct
 * @ifr: ioctl data
 *
 * Set hardware to requested mode. If unsupported, return an error with no
 * changes. Otherwise, store the mode for future reference.
 */
int xlnid_ptp_set_ts_config(struct xlnid_adapter *adapter, struct ifreq *ifr)
{
	struct hwtstamp_config config;
	int err;

	if (copy_from_user(&config, ifr->ifr_data, sizeof(config)))
		return -EFAULT;

	err = xlnid_ptp_set_timestamp_mode(adapter, &config);
	if (err)
		return err;

	/* save these settings for future reference */
	memcpy(&adapter->tstamp_config, &config,
	       sizeof(adapter->tstamp_config));

	return copy_to_user(ifr->ifr_data, &config, sizeof(config)) ?
		-EFAULT : 0;
}

static void xlnid_ptp_link_speed_adjust(struct xlnid_adapter *adapter,
					u32 *shift, u32 *incval)
{
	/**
	 * Scale the NIC cycle counter by a large factor so that
	 * relatively small corrections to the frequency can be added
	 * or subtracted. The drawbacks of a large factor include
	 * (a) the clock register overflows more quickly, (b) the cycle
	 * counter structure must be able to convert the systime value
	 * to nanoseconds using only a multiplier and a right-shift,
	 * and (c) the value must fit within the timinca register space
	 * => math based on internal DMA clock rate and available bits
	 *
	 * Note that when there is no link, internal DMA clock is same as when
	 * link speed is 10Gb. Set the registers correctly even when link is
	 * down to preserve the clock setting
	 */
	switch (adapter->link_speed) {
	case XLNID_LINK_SPEED_100_FULL:
		*shift = XLNID_INCVAL_SHIFT_100;
		*incval = XLNID_INCVAL_100;
		break;
	case XLNID_LINK_SPEED_1GB_FULL:
		*shift = XLNID_INCVAL_SHIFT_1GB;
		*incval = XLNID_INCVAL_1GB;
		break;
	case XLNID_LINK_SPEED_10GB_FULL:
	default:
		*shift = XLNID_INCVAL_SHIFT_10GB;
		*incval = XLNID_INCVAL_10GB;
		break;
	}

	return;
}

/**
 * xlnid_ptp_start_cyclecounter - create the cycle counter from hw
 * @adapter: pointer to the adapter structure
 *
 * This function should be called to set the proper values for the TIMINCA
 * register and tell the cyclecounter structure what the tick rate of SYSTIME
 * is. It does not directly modify SYSTIME registers or the timecounter
 * structure. It should be called whenever a new TIMINCA value is necessary,
 * such as during initialization or when the link speed changes.
 */
void xlnid_ptp_start_cyclecounter(struct xlnid_adapter *adapter)
{
//	struct xlnid_hw *hw = &adapter->hw;
	unsigned long flags;
	struct cyclecounter cc;
	u32 incval = 0;

	/* For some of the boards below this mask is technically incorrect.
	 * The timestamp mask overflows at approximately 61bits. However the
	 * particular hardware does not overflow on an even bitmask value.
	 * Instead, it overflows due to conversion of upper 32bits billions of
	 * cycles. Timecounters are not really intended for this purpose so
	 * they do not properly function if the overflow point isn't 2^N-1.
	 * However, the actual SYSTIME values in question take ~138 years to
	 * overflow. In practice this means they won't actually overflow. A
	 * proper fix to this problem would require modification of the
	 * timecounter delta calculations.
	 */
	cc.mask = CLOCKSOURCE_MASK(64);
	cc.mult = 1;
	cc.shift = 0;

    cc.read = xlnid_ptp_read_skylake;

	xlnid_ptp_link_speed_adjust(adapter, &cc.shift, &incval);
	incval >>= XLNID_INCVAL_SHIFT;
	cc.shift -= XLNID_INCVAL_SHIFT;
	//XLNID_WRITE_REG(hw, XLNID_TIMINCA, (1 << XLNID_INCPER_SHIFT) | incval);

	/* update the base incval used to calculate frequency adjustment */
	WRITE_ONCE(adapter->base_incval, incval);
	smp_mb();

	/* need lock to prevent incorrect read while modifying cyclecounter */
	spin_lock_irqsave(&adapter->tmreg_lock, flags);
	memcpy(&adapter->hw_cc, &cc, sizeof(adapter->hw_cc));
	spin_unlock_irqrestore(&adapter->tmreg_lock, flags);
}

/**
 * xlnid_ptp_reset
 * @adapter: the xlnid private board structure
 *
 * When the MAC resets, all of the hardware configuration for timesync is
 * reset. This function should be called to re-enable the device for PTP,
 * using the last known settings. However, we do lose the current clock time,
 * so we fallback to resetting it based on the kernel's realtime clock.
 *
 * This function will maintain the hwtstamp_config settings, and it retriggers
 * the SDP output if it's enabled.
 */
void xlnid_ptp_reset(struct xlnid_adapter *adapter)
{
//	struct xlnid_hw *hw = &adapter->hw;
	unsigned long flags;

	/* reset the hardware timestamping mode */
	xlnid_ptp_set_timestamp_mode(adapter, &adapter->tstamp_config);

	xlnid_ptp_start_cyclecounter(adapter);

	spin_lock_irqsave(&adapter->tmreg_lock, flags);
	timecounter_init(&adapter->hw_tc, &adapter->hw_cc,
			 ktime_to_ns(ktime_get_real()));
	spin_unlock_irqrestore(&adapter->tmreg_lock, flags);

	adapter->last_overflow_check = jiffies;

	/*
	 * Now that the shift has been calculated and the systime
	 * registers reset, (re-)enable the Clock out feature
	 */
	if (adapter->ptp_setup_sdp)
		adapter->ptp_setup_sdp(adapter);
}

/**
 * xlnid_ptp_create_clock
 * @adapter: the xlnid private adapter structure
 *
 * This functino performs setup of the user entry point function table and
 * initalizes the PTP clock device used by userspace to access the clock-like
 * features of the PTP core. It will be called by xlnid_ptp_init, and may
 * re-use a previously initialized clock (such as during a suspend/resume
 * cycle).
 */

static long xlnid_ptp_create_clock(struct xlnid_adapter *adapter)
{
	struct net_device *netdev = adapter->netdev;
	long err;

	/* do nothing if we already have a clock device */
	if (!IS_ERR_OR_NULL(adapter->ptp_clock))
		return 0;

	snprintf(adapter->ptp_caps.name,
	    sizeof(adapter->ptp_caps.name),
		"%s", netdev->name);
	adapter->ptp_caps.owner = THIS_MODULE;
	adapter->ptp_caps.max_adj = 250000000;
	adapter->ptp_caps.n_alarm = 0;
	adapter->ptp_caps.n_ext_ts = 0;
	adapter->ptp_caps.n_per_out = 0;
	adapter->ptp_caps.pps = 0;
	adapter->ptp_caps.adjfreq = xlnid_ptp_adjfreq_skylake;
	adapter->ptp_caps.adjtime = xlnid_ptp_adjtime_timecounter;
#ifdef HAVE_PTP_CLOCK_INFO_GETTIME64
	adapter->ptp_caps.gettime64 = xlnid_ptp_gettime64_timecounter;
	adapter->ptp_caps.settime64 = xlnid_ptp_settime64_timecounter;
#else
	adapter->ptp_caps.gettime = xlnid_ptp_gettime_timecounter;
	adapter->ptp_caps.settime = xlnid_ptp_settime_timecounter;
#endif
	adapter->ptp_caps.enable = xlnid_ptp_feature_enable;

#if 0
	adapter->ptp_clock = ptp_clock_register(&adapter->ptp_caps,
						pci_dev_to_dev(adapter->pdev));
#endif

	if (IS_ERR(adapter->ptp_clock)) {
		err = PTR_ERR(adapter->ptp_clock);
		adapter->ptp_clock = NULL;
		e_dev_err("ptp_clock_register failed\n");
		return err;
	} else if (adapter->ptp_clock)
		e_dev_info("registered PHC device on %s\n", netdev->name);

	/* Set the default timestamp mode to disabled here. We do this in
	 * create_clock instead of initialization, because we don't want to
	 * override the previous settings during a suspend/resume cycle.
	 */
	adapter->tstamp_config.rx_filter = HWTSTAMP_FILTER_NONE;
	adapter->tstamp_config.tx_type = HWTSTAMP_TX_OFF;

	return 0;
}

/**
 * xlnid_ptp_init
 * @adapter: the xlnid private adapter structure
 *
 * This function performs the required steps for enabling ptp
 * support. If ptp support has already been loaded it simply calls the
 * cyclecounter init routine and exits.
 */
void xlnid_ptp_init(struct xlnid_adapter *adapter)
{
	/* initialize the spin lock first, since the user might call the clock
	 * functions any time after we've initialized the ptp clock device.
	 */
	spin_lock_init(&adapter->tmreg_lock);

	/* obtain a ptp clock device, or re-use an existing device */
	if (xlnid_ptp_create_clock(adapter))
		return;

	/* we have a clock, so we can intialize work for timestamps now */
	INIT_WORK(&adapter->ptp_tx_work, xlnid_ptp_tx_hwtstamp_work);

	/* reset the ptp related hardware bits */
	xlnid_ptp_reset(adapter);

	/* enter the XLNID_PTP_RUNNING state */
	set_bit(__XLNID_PTP_RUNNING, &adapter->state);

	return;
}

/**
 * xlnid_ptp_suspend - stop ptp work items
 * @adapter: pointer to adapter struct
 *
 * This function suspends ptp activity, and prevents more work from being
 * generated, but does not destroy the clock device.
 */
void xlnid_ptp_suspend(struct xlnid_adapter *adapter)
{
	/* leave the XLNID_PTP_RUNNING STATE */
	if (!test_and_clear_bit(__XLNID_PTP_RUNNING, &adapter->state))
		return;

	adapter->flags2 &= ~XLNID_FLAG2_PTP_PPS_ENABLED;
	if (adapter->ptp_setup_sdp)
		adapter->ptp_setup_sdp(adapter);

	cancel_work_sync(&adapter->ptp_tx_work);
	xlnid_ptp_clear_tx_timestamp(adapter);
}

/**
 * xlnid_ptp_stop - destroy the ptp_clock device
 * @adapter: pointer to adapter struct
 *
 * Completely destroy the ptp_clock device, and disable all PTP related
 * features. Intended to be run when the device is being closed.
 */
void xlnid_ptp_stop(struct xlnid_adapter *adapter)
{
	/* first, suspend ptp activity */
	xlnid_ptp_suspend(adapter);

	/* now destroy the ptp clock device */
	if (adapter->ptp_clock) {
		//ptp_clock_unregister(adapter->ptp_clock);
		adapter->ptp_clock = NULL;
		e_dev_info("removed PHC on %s\n",
			   adapter->netdev->name);
	}
}
