/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (c) 2015 - 2022 Beijing WangXun Technology Co., Ltd. */
#ifndef _TXGBE_BP_H_
#define _TXGBE_BP_H_

#include "txgbe.h"
#include "txgbe_type.h"
#include "txgbe_hw.h"

/* Backplane AN73 Base Page Ability struct*/
typedef struct TBKPAN73ABILITY {
	unsigned int nextPage;    //Next Page (bit0)
	unsigned int linkAbility; //Link Ability (bit[7:0])
	unsigned int fecAbility;  //FEC Request (bit1), FEC Enable  (bit0)
	unsigned int currentLinkMode; //current link mode for local device
} bkpan73ability;

#ifndef read_poll_timeout
#define read_poll_timeout(op, val, cond, sleep_us, timeout_us, \
				sleep_before_read, args...) \
({ \
	u64 __timeout_us = (timeout_us); \
	unsigned long __sleep_us = (sleep_us); \
	ktime_t __timeout = ktime_add_us(ktime_get(), __timeout_us); \
	might_sleep_if((__sleep_us) != 0); \
	if (sleep_before_read && __sleep_us) \
		usleep_range((__sleep_us >> 2) + 1, __sleep_us); \
	for (;;) { \
		(val) = op(args); \
		if (cond) \
			break; \
		if (__timeout_us && \
		    ktime_compare(ktime_get(), __timeout) > 0) { \
			(val) = op(args); \
			break; \
		} \
		if (__sleep_us) \
			usleep_range((__sleep_us >> 2) + 1, __sleep_us); \
		cpu_relax(); \
	} \
	(cond) ? 0 : -ETIMEDOUT; \
})
#endif

#define kr_dbg(KR_MODE, fmt, arg...) \
	do { \
		if (KR_MODE) \
			e_dev_info(fmt, ##arg); \
	} while (0)

void txgbe_bp_down_event(struct txgbe_adapter *adapter);
void txgbe_bp_watchdog_event(struct txgbe_adapter *adapter);
int txgbe_bp_mode_setting(struct txgbe_adapter *adapter);
void txgbe_bp_close_protect(struct txgbe_adapter *adapter);
int handle_bkp_an73_flow(unsigned char bp_link_mode, struct txgbe_adapter *adapter);
int get_bkp_an73_ability(bkpan73ability *pt_bkp_an73_ability, unsigned char byLinkPartner,
			 struct txgbe_adapter *adapter);
int clr_bkp_an73_int(unsigned int intIndex, unsigned int intIndexHi,
		     struct txgbe_adapter *adapter);
int chk_bkp_an73_ability(bkpan73ability tBkpAn73Ability, bkpan73ability tLpBkpAn73Ability,
			 struct txgbe_adapter *adapter);
#endif

