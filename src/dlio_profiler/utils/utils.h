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
const std::string ignore_filenames[5] = {".pfw", "/pipe", "/socket","/proc/self", ".py"};
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
inline bool ignore_files(const char* filename) {
  for(auto &file: ignore_filenames) {
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

inline std::pair<bool, std::string> is_traced_common(const char* filename, const char* func,
                                              const std::vector<std::string>& ignore_filename,
                                              const std::vector<std::string>& track_filename) {
  bool found = false;
  bool ignore = false;
  char resolved_path[PATH_MAX];
  char* data = realpath(filename, resolved_path);
  (void) data;
  if (ignore_files(resolved_path) || ignore_files(filename)) {
    DLIO_PROFILER_LOGINFO("Profiler ignoring file %s for func %s", resolved_path, func);
    return std::pair<bool, std::string>(false, filename);
  }
  for (const auto file : ignore_filename) {
    if (strstr(resolved_path, file.c_str()) != NULL) {
      DLIO_PROFILER_LOGINFO("Profiler Intercepted POSIX not file %s for func %s", resolved_path, func);
      ignore = true;
      break;
    }
  }
  if (!ignore) {
    for (const auto file : track_filename) {
      if (strstr(resolved_path, file.c_str()) != NULL) {
        DLIO_PROFILER_LOGINFO("Profiler Intercepted POSIX tracing file %s for func %s", resolved_path, func);
        found = true;
        break;
      }
    }
  }
  if (!found and !ignore) DLIO_PROFILER_LOGINFO("Profiler Intercepted POSIX not tracing file %s for func %s", resolved_path, func);
  return std::pair<bool, std::string>(found, filename);
}
#endif // DLIO_PROFILER_UTILS_H
