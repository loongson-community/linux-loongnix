// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2022 - 2023 Mucse Corporation. */
#ifndef RNMP_MPE_H
#define RNMP_MPE_H

#include "rnpm.h"

int rnpm_rpu_mpe_start(struct rnpm_pf_adapter *adapter);
void rnpm_rpu_mpe_stop(struct rnpm_pf_adapter *adapter);

#endif // RNMP_MPE
