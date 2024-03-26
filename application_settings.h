/*
 * Copyright (C) 2021-2024, HENSOLDT Cyber GmbH
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * For commercial licensing, contact: info.cyber@hensoldt.net
 */

#define GET_INSTANCE(_idx_) \
    instance_ ## _idx_

#define GET_LOCK(_idx_) \
    lock_ ## _idx_

#define GET_PROC(_idx_) \
    proc_ ## _idx_

#define SAME_PRIOS_SAME_CORES   0
#define SAME_PRIOS_DIFF_CORES   1
#define DIFF_PRIOS_SAME_CORES   2
#define DIFF_PRIOS_DIFF_CORES   3
#define APP_MODE                SAME_PRIOS_SAME_CORES

#define NUM_ITERATIONS          100000

#define NUM_INSTANCES           1

#if NUM_INSTANCES <= 0 || NUM_INSTANCES > 4
#pragma message "Illegal number of instances!"
#endif