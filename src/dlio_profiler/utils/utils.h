//
// Created by haridev on 8/23/23.
//

#ifndef DLIO_PROFILER_UTILS_H
#define DLIO_PROFILER_UTILS_H

#include <string>
#include <limits.h>
#include <dlio_profiler/utils/posix_internal.h>
#include <vector>
#include <vector>
#include <cstring>
#include <dlio_profiler/core/macro.h>
#include <iostream>
#include <execinfo.h>
#include <sstream>
#include <regex>

void dlio_finalize();

inline void signal_handler(int sig) {  // GCOVR_EXCL_START
  switch (sig) {
    case SIGTERM: {
      DLIO_PROFILER_LOGDEBUG("terminate signal caught", 0);
      dlio_finalize();
      exit(1);
      break;
    }
    default: {
      DLIO_PROFILER_LOGINFO("signal caught %d", sig);
      dlio_finalize();
      int j, nptrs;
      void *buffer[20];
      char **strings;

      nptrs = backtrace(buffer, 20);
      strings = backtrace_symbols(buffer, nptrs);
      if (strings != NULL) {
        for (j = 0; j < nptrs; j++)
          printf("%s\n", strings[j]);

        free(strings);
      }
      ::raise(SIGTERM);
    }

  }
} // GCOVR_EXCL_STOP

inline void set_signal() {
  struct sigaction sa;
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sigaction(SIGSEGV, &sa, NULL);
  sigaction(SIGUSR1, &sa, NULL);
  sigaction(SIGABRT, &sa, NULL);
  sigaction(SIGHUP, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);
  sigaction(SIGINT, &sa, NULL);
}  // GCOVR_EXCL_STOP

const std::string ignore_filenames[5] = {".pfw", "/pipe", "/socket", "/proc/self", ".py"};

inline std::vector<std::string> split(std::string str, char delimiter) {
  std::vector<std::string> res;
  if (str.find(delimiter) == std::string::npos) {
    res.push_back(str);
  } else {
    size_t first;
    size_t last = 0;
    while ((first = str.find_first_not_of(delimiter, last)) != std::string::npos) {
      last = str.find(delimiter, first);
      res.push_back(str.substr(first, last - first));
    }
  }
  return res;
}

inline bool ignore_files(const char *filename) {
  for (auto &file: ignore_filenames) {
    if (strstr(filename, file.c_str()) != NULL) {
      return true;
    }
  }
  return false;
}

inline std::string get_filename(int fd) {
  char proclnk[PATH_MAX];
  char filename[PATH_MAX];
  snprintf(proclnk, PATH_MAX, "/proc/self/fd/%d", fd);
  size_t r = dlp_readlink(proclnk, filename, PATH_MAX);
  filename[r] = '\0';
  return filename;
}

inline std::pair<bool, std::string> is_traced_common(const char *filename, const char *func,
                                                     const std::vector<std::string> &ignore_filename,
                                                     const std::vector<std::string> &track_filename) {
  bool found = false;
  bool ignore = false;
  char resolved_path[PATH_MAX];
  char *data = realpath(filename, resolved_path);
  (void) data;
  if (ignore_files(resolved_path) || ignore_files(filename)) {
    DLIO_PROFILER_LOGDEBUG("Profiler ignoring file %s for func %s", resolved_path, func);
    return std::pair<bool, std::string>(false, resolved_path);
  }
  for (const auto file : ignore_filename) {
    if (strstr(resolved_path, file.c_str()) != NULL) {
      ignore = true;
      break;
    }
  }
  if (!ignore) {
    for (const auto file : track_filename) {
      if (strstr(resolved_path, file.c_str()) != NULL) {
        found = true;
        break;
      }
    }
  }
  if (!found && !ignore) {
    DLIO_PROFILER_LOGDEBUG("Profiler Intercepted POSIX not tracing file %s for func %s", resolved_path, func);
  }
  else {
    DLIO_PROFILER_LOGWARN("Profiler Intercepted POSIX tracing file %s for func %s", resolved_path, func);
  }
  return std::pair<bool, std::string>(found, resolved_path);
}

#endif // DLIO_PROFILER_UTILS_H
