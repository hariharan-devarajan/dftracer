//
// Created by haridev on 3/28/23.
//

#ifndef DLIO_PROFILER_ENUMERATION_H
#define DLIO_PROFILER_ENUMERATION_H
enum WriterType : uint8_t {
    CHROME = 0
};
enum ProfilerStage : uint8_t {
    PROFILER_INIT = 0, PROFILER_FINI = 1, PROFILER_OTHER = 2
};
enum ProfileType : uint8_t {
    PROFILER_PRELOAD = 0, PROFILER_PY_APP = 1, PROFILER_CPP_APP = 2, PROFILER_C_APP = 3, PROFILER_ANY = 4
};
#endif //DLIO_PROFILER_ENUMERATION_H
