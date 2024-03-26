/*
 * Copyright (C) 2019-2024, HENSOLDT Cyber GmbH
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * For commercial licensing, contact: info.cyber@hensoldt.net
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include <camkes.h>

#include "../../application_settings.h"

#include "lib_debug/Debug.h"
#include "TimeServer.h"

#define SEED    35791246

static const if_OS_Timer_t timer =
    IF_OS_TIMER_ASSIGN(
        timeServer_rpc,
        timeServer_notify);

// See: https://www.geeksforgeeks.org/estimating-value-pi-using-monte-carlo/
void calculate_pi(unsigned int iter, unsigned int seed)
{
    int square_points = 0;
    int circle_points = 0;

    srand(seed);
    for (size_t i = 0; i < iter; i++)
    {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        double z = x * x + y * y;
        if (z <= 1)
        {
            circle_points++;
        }
        square_points++;
    }
    double pi= 4 * ((double)circle_points / (double)square_points);
    lock_lock();
    Debug_LOG_DEBUG("# of trials: %d, estimate of pi: %g!",iter,pi);
    lock_unlock();
    return;
}

int run()
{
    setbuf(stdout,NULL);
/*
 * The idea here would be to use a ARM per-core timer to get the timer value
 * instead of the TimeServer. Calls to the timeserver take time and might distort
 * the accuracy of the multicore demo.
 * The registers used by the assembly instructions need to be platform specific.
 */
// #ifdef AARCH64
// https://developer.arm.com/documentation/102412/0100/Privilege-and-Exception-levels
// https://developer.arm.com/documentation/102379/0000/The-processor-timers?lang=en
// https://www.keil.com/support/man/docs/armasm/armasm_dom1361289881374.htm
//     Debug_LOG_DEBUG("aarch64");
//     uint64_t TIME_REG;
//     asm volatile ("mrs %0, CNTP_TVAL_EL0" : "=r" (TIME_REG));
// #else
// https://developer.arm.com/documentation/ddi0438/i/generic-timer/generic-timer-programmers-modela
// https://www.keil.com/support/man/docs/armasm/armasm_dom1361289880404.htm
//     Debug_LOG_DEBUG("aarch32");
//     uint32_t CNTVAL;
//     asm volatile ("mrc p15, 0, %0, c14, c0, 0" : "=r" (CNTVAL));
// #endif
    // seL4_DebugDumpScheduler();

    uint64_t time_a = 0;
    uint64_t time_b = 0;

    lock_lock();
    Debug_LOG_DEBUG("%s: entering...",get_instance_name());
    TimeServer_getTime(&timer,TimeServer_PRECISION_NSEC,&time_a);
    if(proc_get_entered() == 0)
    {
        proc_set_time_a(time_a);
        proc_increment_entered();
    }
    lock_unlock();

    calculate_pi(NUM_ITERATIONS,SEED);

    lock_lock();
    Debug_LOG_DEBUG("%s: leaving...",get_instance_name());
    TimeServer_getTime(&timer,TimeServer_PRECISION_NSEC,&time_b);
    Debug_LOG_DEBUG("%s: Pi was calculated in %" PRIu64 " ns.",
                        get_instance_name(),
                        time_b - time_a);
    proc_increment_left();
    if(proc_get_left() == NUM_INSTANCES)
    {
        proc_set_time_b(time_b);
        uint64_t delta = proc_get_time_b() - proc_get_time_a();
        Debug_LOG_DEBUG("General: Pi was calculated in %" PRIu64 " ns", delta);
    }
    lock_unlock();

    return 0;
}
