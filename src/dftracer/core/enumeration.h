//
// Created by haridev on 3/28/23.
//

#ifndef DFTRACER_ENUMERATION_H
#define DFTRACER_ENUMERATION_H
enum WriterType : uint8_t { CHROME = 0 };
enum ProfilerStage : uint8_t {
  PROFILER_INIT = 0,
  PROFILER_FINI = 1,
  PROFILER_OTHER = 2
};
enum ProfileType : uint8_t {
  PROFILER_PRELOAD = 0,
  PROFILER_PY_APP = 1,
  PROFILER_CPP_APP = 2,
  PROFILER_C_APP = 3,
  PROFILER_ANY = 4
};
enum ProfileInitType : uint8_t {
  PROFILER_INIT_NONE = 0,
  PROFILER_INIT_LD_PRELOAD = 1,
  PROFILER_INIT_FUNCTION = 2
};
inline void convert(const std::string &s, ProfileInitType &type) {
  if (s == "PRELOAD") {
    type = ProfileInitType::PROFILER_INIT_LD_PRELOAD;
  } else if (s == "FUNCTION") {
    type = ProfileInitType::PROFILER_INIT_FUNCTION;
  } else {
    type = ProfileInitType::PROFILER_INIT_NONE;
  }
}

inline void convert(const std::string &s, cpplogger::LoggerType &type) {
  if (s == "DEBUG") {
    type = cpplogger::LoggerType::LOG_DEBUG;
  } else if (s == "INFO") {
    type = cpplogger::LoggerType::LOG_INFO;
  } else if (s == "WARN") {
    type = cpplogger::LoggerType::LOG_WARN;
  } else {
    type = cpplogger::LoggerType::LOG_ERROR;
  }
}
#endif  // DFTRACER_ENUMERATION_H
