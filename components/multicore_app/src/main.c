/**
 * Copyright (C) 2019-2020, Hensoldt Cyber GmbH
 */

#include <stdio.h>

#include <camkes.h>

#include "TimeServer.h"

static const if_OS_Timer_t timer =
    IF_OS_TIMER_ASSIGN(
        timeServer_rpc,
        timeServer_notify);

void calculate_pi(void)
{
    int r[2800 + 1];
    int i, k;
    int b, d;
    int c = 0;

    for (i = 0; i < 2800; i++) {
        r[i] = 2000;
    }

    for (k = 2800; k > 0; k -= 14) {
        d = 0;

        i = k;
        for (;;) {
            d += r[i] * 10000;
            b = 2 * i - 1;

            r[i] = d % b;
            d /= b;
            i--;
            if (i == 0) break;
            d *= i;
        }
        // printf("%.4d", c + d / 10000);
        (void)c;
        c = d % 10000;
    }
}

int run()
{
    setbuf(stdout,NULL);
    seL4_DebugDumpScheduler();

    uint64_t time_a = 0;
    uint64_t time_b = 0;
    TimeServer_getTime(&timer,TimeServer_PRECISION_NSEC,&time_a);
    calculate_pi();
    TimeServer_getTime(&timer,TimeServer_PRECISION_NSEC,&time_b);
    printf("Pi was calculated in %lld ns \n",time_b - time_a);
    return 0;
}
