/* SPDX-License-Identifier: GPL-2.0 */
/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1999, 2000, 06 Ralf Baechle (ralf@linux-mips.org)
 * Copyright (C) 1999, 2000 Silicon Graphics, Inc.
 * Copyright (C) 2020 Loongson Technology Corporation Limited
 */
#ifndef _ASM_SPINLOCK_H
#define _ASM_SPINLOCK_H

#include <asm/processor.h>
#include <asm/qrwlock.h>

#include <asm-generic/qspinlock_types.h>

#define queued_spin_unlock queued_spin_unlock
/**
 * queued_spin_unlock - release a queued spinlock
 * @lock : Pointer to queued spinlock structure
 */
static inline void queued_spin_unlock(struct qspinlock *lock)
{
	compiletime_assert_atomic_type(lock->locked);
	c_sync();
	WRITE_ONCE(lock->locked, 0);
}

#include <asm/qspinlock.h>

#endif /* _ASM_SPINLOCK_H */
