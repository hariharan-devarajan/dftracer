//
// Created by haridev on 3/28/23.
//

#ifndef DFTRACER_DFTRACER_PRELOAD_H
#define DFTRACER_DFTRACER_PRELOAD_H

extern void __attribute__((constructor)) dftracer_init(void);

extern void __attribute__((destructor)) dftracer_fini(void);

#endif  // DFTRACER_DFTRACER_PRELOAD_H
