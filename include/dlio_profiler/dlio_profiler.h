//
// Created by haridev on 3/28/23.
//

#ifndef DLIO_PROFILER_DLIO_PROFILER_H
#define DLIO_PROFILER_DLIO_PROFILER_H
#include <brahma/brahma.h>
#include <dlio_profiler/brahma/posix.h>
#include <dlio_profiler/brahma/stdio.h>

extern void __attribute__ ((constructor)) dlio_profiler_init(void);
extern void __attribute__ ((destructor)) dlio_profiler_fini(void);

#endif //DLIO_PROFILER_DLIO_PROFILER_H
